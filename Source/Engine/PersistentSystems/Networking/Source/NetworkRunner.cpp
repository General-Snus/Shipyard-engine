
#define NOMINMAX
#include "PersistentSystems.pch.h"


#include "../NetworkRunner.h" 
#include "Engine/PersistentSystems/Networking/NetMessage/StringMessages.h" 
#include "Engine/PersistentSystems/Networking/NetMessage/PlayerSyncMessage.h"  
#include "Editor/Editor/Windows/Window.h"
#include "Tools\Utilities\Game\Timer.h"

#undef GetMessage
#undef GetMessageW

bool InitializeWinsock()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result == 0)
	{
		return true;
	}
	return false;
}

void CleanupWinsock()
{
	WSACleanup();
}

NetworkRunner::NetworkRunner() = default;
NetworkRunner::~NetworkRunner()
{
	connection.Close(); CleanupWinsock();
};

NetworkConnection::Status NetworkRunner::StartSession(SessionConfiguration configuration)
{
	OPTICK_EVENT();

	if (!InitializeWinsock())
	{
		LOGGER.ErrTrace("WINSOCK INIT FAILED");
		return NetworkConnection::Status::failed;;
	}

	Close();

	//Refresh uuid
	runnerID = NetworkedId::Generate();


	NetworkConnection::Status status = NetworkConnection::Status::failed;

	switch (configuration.gameMode)
	{
	case SessionConfiguration::GameMode::Single:
		status = NetworkConnection::Status::initialized;
		HasStateAuthority = true;
		HasInputAuthority = true;
		return status;
	case SessionConfiguration::GameMode::Shared:
		assert("Implement");
	case SessionConfiguration::GameMode::Server:
	case SessionConfiguration::GameMode::Host:
		status = connection.StartAsServer(configuration.address, configuration);
		break;
	case SessionConfiguration::GameMode::Client:
		status = connection.StartAsClient(configuration.address, configuration);
		break;
	case SessionConfiguration::GameMode::AutoHostOrClient:
		status = connection.AutoHostOrClient(configuration.address, configuration);
		break;
	default:
		status = NetworkConnection::Status::failed;
		break;
	}
	Initialized = (status && NetworkConnection::Status::initialized);

	if (status == NetworkConnection::Status::initializedAsServer)
	{
		IsServer = true;
		HasStateAuthority = true;
		HasInputAuthority = true;

		//Start acceptance thread
		if (receiveTCP.joinable())
		{ // Server uses tcp for accaptence, servers tcp newConnection to client is handeled in the remotes
			receiveTCP.request_stop();
			receiveTCP.join();
		}
		receiveTCP = std::jthread([this](std::stop_token s) { this->acceptNewClients(s); });
		WindowInstance.SetWindowsTitle("SERVER");
	}

	if (status == NetworkConnection::Status::initializedAsClient)
	{
		IsServer = false;
		HasStateAuthority = false;
		HasInputAuthority = true;
		serverUdpConnection = NetAddress();

		if (receiveTCP.joinable())
		{ // Client listens in on tcp on this thread
			receiveTCP.request_stop();
			receiveTCP.join();
		}
		receiveTCP = std::jthread([this](std::stop_token s)
		{
			this->collectReceivedMessages(s, runnerID, NetworkConnection::Protocol::TCP);
		});

		HandshakeMessage message(runnerID);
		message.SetMessage(runnerName);
		Send(message, NetworkConnection::Protocol::TCP);
		WindowInstance.SetWindowsTitle("CLIENT");
	}


	if (Initialized)
	{ 
		//Both server and client should listen to UDP
		if (receiveUDP.joinable())
		{
			receiveUDP.request_stop();
			receiveUDP.join();
		}

		receiveUDP = std::jthread([this](std::stop_token s)
		{
			this->collectReceivedMessages(s, runnerID, NetworkConnection::Protocol::UDP);
		});
	}

	return status;
}

void NetworkRunner::Update()
{
	OPTICK_EVENT();

	sentDataPerFrame = 0;
	readDataPerFrame = 0;
	for (auto& remote : remoteConnections)
	{
		remote.readDataPerFrame = 0;
		remote.sentDataPerFrame = 0;
	}

	moveMessageMapToRead();
	heartBeatSystem.Update(*this);

	//Handle runtime messages
	layer.fixedNetworkUpdate(*this);
	if (!IsServer)
	{
		return;
	}

	//Server processes custom messages here
	for (auto& [type, messagesToProcess] : messagesMap)
	{
		for (auto& incomingMessage : messagesToProcess)
		{
			ProcessIncoming(type, incomingMessage);
		}
	}
}

void NetworkRunner::ProcessIncoming(const eNetMessageType& type, RecievedMessage& incomingMessage)
{
	OPTICK_EVENT();
	switch (type)
	{
	case eNetMessageType::ChatMessage:
	{
		const auto          message = std::bit_cast<ChatMessage>(incomingMessage.message);
		ChatMessage outMessage(message.GetId());
		outMessage.SetMessage(NameOfConnection(message.GetId()) + ": " + message.ReadMessage());
		incomingMessage.message = static_cast<NetMessage>(outMessage);
		Broadcast(outMessage, NetworkConnection::Protocol::TCP);
	}
	break;
	case eNetMessageType::NewConnection:
	{
		const auto          message = std::bit_cast<HandshakeMessage>(incomingMessage.message);
		if (incomingMessage.idFrom == runnerID)
		{
			runnerName = message.ReadMessage();
			break;
		}

		if (auto* remote = IdToRemote(incomingMessage.idFrom))
		{
			remote->nickname = message.ReadMessage();
		}

		ChatMessage outMessage(message.GetId());
		outMessage.SetMessage(NameOfConnection(message.GetId()) + " Joined!");
		Broadcast(outMessage, NetworkConnection::Protocol::TCP);

		/*
		Custom handel this in the context of the remote
		auto* remote = IdToRemote(incomingMessage.idFrom);
		if(remote != nullptr) {
		remote->id = message.GetId();
		remote->nickname = message.ReadMessage();
		}*/
	}
	break;
	case eNetMessageType::NicknameSet:
	{
		const auto message = std::bit_cast<NicknameMessage>(incomingMessage.message);
		if (incomingMessage.idFrom == runnerID)
		{
			runnerName = message.ReadMessage();
			break;
		}

		if (auto* remote = IdToRemote(incomingMessage.idFrom))
		{
			remote->nickname = message.ReadMessage();
		}
	}
	break;
	case eNetMessageType::Quit:
	{
		const auto          message = std::bit_cast<QuitMessage>(incomingMessage.message);
		ChatMessage outMessage(message.GetId());
		outMessage.SetMessage(NameOfConnection(message.GetId()) + " Left the server");
		Broadcast(outMessage, NetworkConnection::Protocol::TCP);

		auto* remote = IdToRemote(incomingMessage.idFrom);
		if (remote != nullptr)
		{
			remote->Close();
		}
	}
	break;
	case eNetMessageType::Count:
		break;
	default:
		//For all not custom proccessed messages, sendem to everyone
		Broadcast(incomingMessage.message, incomingMessage.protocol);
		break;
	}
}

void NetworkRunner::Close()
{
	OPTICK_EVENT();
	for (auto& remote : remoteConnections)
	{
		remote.Close();
	}

	layer.Close();
	receiveTCP.request_stop();
	receiveUDP.request_stop();
	connection.Close();

	IsServer = false;
	HasStateAuthority = false;
	HasInputAuthority = false;
}

bool NetworkRunner::Send(NetMessage& message, NetworkConnection::Protocol protocol)
{
	OPTICK_EVENT();
	if (!Initialized)
	{
		return false;
	}
	message.Id = runnerID;

	if (IsServer)
	{
		std::scoped_lock lock(addMessageLock);
		RecievedMessage recv{
			message,
			connection.Address(),
			runnerID,
			protocol
		};

		threadedMessageMap[message.myType].emplace_back(recv);
		return true;
	}
	else
	{
		sentDataPerFrame += sizeof(message);
		//Connection is bound to server 
		switch (protocol)
		{
		case NetworkConnection::Protocol::UDP:
			return connection.SendUDP(serverUdpConnection, message);
			break;
		case NetworkConnection::Protocol::TCP:
			return connection.SendTCP(message);
			break;
		default:
			std::unreachable();
			break;
		}
	}
}

bool NetworkRunner::SendTo(const Remote& remote, NetMessage& message, NetworkConnection::Protocol protocol) const
{
	remote.sentDataPerFrame += sizeof(message);
	switch (protocol)
	{
	case NetworkConnection::Protocol::UDP:
		return connection.SendUDP(remote.udpAddress, message);
		break;
	case NetworkConnection::Protocol::TCP:
		return remote.remoteConnection.SendTCP(message);
		break;
	default:
		std::unreachable();
		break;
	}
}

bool NetworkRunner::SendTo(const NetworkedId& remoteId, NetMessage& message, NetworkConnection::Protocol protocol) const
{
	if (const auto* remote = IdToRemote(remoteId))
	{
		return SendTo(*remote, message, protocol);
	}
	return false;
}

bool NetworkRunner::HasConnection(NetworkedId id)
{
	OPTICK_EVENT();
	if (id == runnerID && connection.GetStatus() == NetworkConnection::Status::initialized)
	{
		return true;
	}

	if (IsServer)
	{
		for (const auto& remote : remoteConnections)
		{
			if (remote.isConnected && remote.id == id)
			{
				return true;
			}
		}
	}

	return false;
}

std::string NetworkRunner::NameOfConnection(NetworkedId id)
{
	OPTICK_EVENT();
	if (id == runnerID && (connection.GetStatus() && NetworkConnection::Status::initialized))
	{
		return runnerName;
	}

	const Remote* remote = IdToRemote(id);
	if (remote != nullptr)
	{
		return remote->nickname;
	}

	return "";
}

Remote* NetworkRunner::IdToRemote(NetworkedId id)
{
	OPTICK_EVENT();
	if (id == runnerID)
	{
		return nullptr;
	}

	if (IsServer)
	{
		for (auto& remote : remoteConnections)
		{
			if (remote.isConnected && remote.id == id)
			{
				return &remote;
			}
		}
	}
	return nullptr;
}

const Remote* NetworkRunner::IdToRemote(NetworkedId id) const
{
	OPTICK_EVENT();
	if (id == runnerID)
	{
		return nullptr;
	}

	if (IsServer)
	{
		for (const auto& remote : remoteConnections)
		{
			if (remote.isConnected && remote.id == id)
			{
				return &remote;
			}
		}
	}
	return nullptr;
}

//The data uplink as server is the combined send on all remotes and its udp socket
//The data uplink of a client is just the runners send of tcp and udp socket
float NetworkRunner::uplinkRate()
{
	if (IsServer)
	{
		int dataUp = 0;
		for (auto& remote : remoteConnections)
		{
			dataUp += remote.sentDataPerFrame;
		}

		return float(dataUp + sentDataPerFrame) * TimerInstance.getDeltaTime();
	}
	else
	{
		return float(sentDataPerFrame) * TimerInstance.getDeltaTime();
	}
}
//The data downlink as server is the combined read of all remotes and its udp socket
//The data downlink of a client is just the runners read of tcp and udp socket
float NetworkRunner::downlinkRate()
{
	if (IsServer)
	{
		int dataUp = 0;
		for (auto& remote : remoteConnections)
		{
			dataUp += remote.readDataPerFrame;
		}

		return float(dataUp + readDataPerFrame) * TimerInstance.getDeltaTime();
	}
	else
	{
		return float(readDataPerFrame) * TimerInstance.getDeltaTime();
	}
}

bool NetworkRunner::registerObject(const NetworkObject& object)
{
	OPTICK_EVENT();
	if (!IsServer)
	{
		return false;
	}

	return layer.registerObject(*this, object);
}

bool NetworkRunner::unRegisterObject(const NetworkObject& object)
{
	OPTICK_EVENT();
	if (!IsServer)
	{
		return false;
	}

	return layer.unRegisterObject(*this, object);
}

bool NetworkRunner::Broadcast(NetMessage& message, NetworkConnection::Protocol protocol) const
{
	OPTICK_EVENT();
	assert(IsServer);
	bool allSucceeded = true;
	for (auto& client : remoteConnections)
	{
		if (!client.isConnected)
		{
			continue;
		}

		switch (protocol)
		{
		case NetworkConnection::Protocol::UDP:
			if (!client.hasConnectedOverUDP)
			{
				return false;
			} // Passthrough!
		case NetworkConnection::Protocol::TCP:
			allSucceeded &= SendTo(client, message, protocol);
			break;
		default:
			std::unreachable();
			break;
		}
	}

	return allSucceeded;
}

bool NetworkRunner::Unicast(NetMessage& message, Remote client, NetworkConnection::Protocol protocol) const
{
	OPTICK_EVENT();
	switch (protocol)
	{
	case NetworkConnection::Protocol::UDP:
		if (!client.hasConnectedOverUDP)
		{
			return false;
		}
		return connection.SendUDP(client.udpAddress, message);
		break;
	case NetworkConnection::Protocol::TCP:
		return connection.SendTCP(message);
		break;
	default:
		std::unreachable();
		break;
	}
}

bool NetworkRunner::Multicast(NetMessage& message, std::span<Remote> spanOfClient, NetworkConnection::Protocol protocol) const
{
	OPTICK_EVENT();
	bool allSucceded = true;
	for (const auto& client : spanOfClient)
	{
		switch (protocol)
		{
		case NetworkConnection::Protocol::UDP:
			if (!client.hasConnectedOverUDP)
			{
				continue;
			}
			allSucceded &= client.remoteConnection.SendUDP(client.udpAddress, message);
			break;
		case NetworkConnection::Protocol::TCP:
			allSucceded &= client.remoteConnection.SendTCP(message);
			break;
		default:
			break;
		}
	}

	return allSucceded;
}

void NetworkRunner::acceptNewClients(std::stop_token stop_token)
{
	NetworkConnection newConnection;
	OPTICK_THREAD("acceptNewClients");
	while (!stop_token.stop_requested())
	{
		if (connection.Accept(&newConnection))
		{
			for (auto& client : remoteConnections)
			{
				if (client.isConnected == false)
				{
					client.Close();
					client.isConnected = true;
					client.remoteConnection = newConnection;
					client.receiveTCP = std::jthread([&client](std::stop_token s)
					{
						client.collectReceivedMessages(s);
					});

					newConnection = {};
					break;
				}
			}
		}
	}
}

void NetworkRunner::collectReceivedMessages(std::stop_token stop_token, NetworkedId recieversOwnId, NetworkConnection::Protocol protocol)
{
	OPTICK_THREAD("NetworkRunner::collectReceivedMessages");
	if (!Initialized)
	{
		return;
	}

	NetMessage incomingMessage;
	NetAddress recievedFromAddress;
	while (!stop_token.stop_requested())
	{
		if (protocol == NetworkConnection::Protocol::UDP)
		{
			if (connection.ReceiveUDP(&incomingMessage, &recievedFromAddress))
			{
				readDataPerFrame += sizeof(incomingMessage);

				if (incomingMessage.Id == recieversOwnId)
				{
					continue;
				}

				std::scoped_lock lock(addMessageLock);

				RecievedMessage recv{
					incomingMessage,
					recievedFromAddress,
					incomingMessage.GetId()
				};

				if (heartBeatSystem.RecieveMessage(*this, recv))
				{
					continue;
				}

				threadedMessageMap[incomingMessage.myType].emplace_back(recv);
			}
		}

		if (protocol == NetworkConnection::Protocol::TCP)
		{
			if (int error = connection.ReceiveTCP(&incomingMessage, &recievedFromAddress) == ReceiveSuccessful)
			{
				readDataPerFrame += sizeof(incomingMessage);

				if (incomingMessage.Id == recieversOwnId)
				{
					continue;
				}

				std::scoped_lock lock(addMessageLock);

				RecievedMessage recv{
					incomingMessage,
					recievedFromAddress,
					incomingMessage.GetId()
				};

				threadedMessageMap[incomingMessage.myType].emplace_back(recv);
			}
			else
			{
				error;

			}
		}
	}
}

void NetworkRunner::moveMessageMapToRead()
{
	OPTICK_EVENT();
	std::scoped_lock lock(addMessageLock);

	for (auto& [type, messageVector] : messagesMap)
	{
		messageVector.clear();
	}

	for (auto& [type, messageVector] : threadedMessageMap)
	{
		messagesMap[type] = messageVector;
		messageVector.clear();
	}

	for (auto& remote : remoteConnections)
	{
		if (remote.isConnected && !remote.messages.empty())
		{
			for (const auto& message : remote.Read())
			{
				assert(remote.id == message.message.GetId());

				RecievedMessage recv{
					message.message,
					message.from,
					remote.id
				};
				messagesMap[message.message.myType].emplace_back(recv);
			}
			remote.Consume();
		}
	}
}
