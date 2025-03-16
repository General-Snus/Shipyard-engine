
#define NOMINMAX
#include "PersistentSystems.pch.h"


#include "../NetworkRunner.h" 
#include "Engine/PersistentSystems/Networking/NetMessage/StringMessages.h" 
#include "Engine/PersistentSystems/Networking/NetMessage/PlayerSyncMessage.h"  
#include "Editor/Editor/Windows/Window.h"

#undef GetMessage
#undef GetMessageW

#pragma optimize( "", off ) 
bool InitializeWinsock()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2),&wsaData);
	if(result == 0)
	{
		return true;
	}
	return false;
}

void CleanupWinsock()
{
	WSACleanup();
}

NetworkRunner::NetworkRunner() {};
NetworkRunner::~NetworkRunner() { connection.Close(); CleanupWinsock(); };

NetworkConnection::Status NetworkRunner::StartSession(SessionConfiguration configuration)
{

	if(!InitializeWinsock())
	{
		LOGGER.ErrTrace("Server init failed");
		return NetworkConnection::Status::failed;;
	}

	Close();

	//Refresh uuid
	runnerID = NetworkedId::Generate();


	NetworkConnection::Status status = NetworkConnection::Status::failed;

	switch(configuration.gameMode)
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
		status = connection.StartAsServer(configuration.address,configuration);
		break;
	case SessionConfiguration::GameMode::Client:
		status = connection.StartAsClient(configuration.address,configuration);
		break;
	case SessionConfiguration::GameMode::AutoHostOrClient:
		status = connection.AutoHostOrClient(configuration.address,configuration);
		break;
	default:
		status = NetworkConnection::Status::failed;
		break;
	}
	Initialized = (status && NetworkConnection::Status::initialized);

	if(status == NetworkConnection::Status::initializedAsServer)
	{
		IsServer = true;
		HasStateAuthority = true;
		HasInputAuthority = true;

		//Start acceptance thread
		if(receiveTCP.joinable())
		{ // Server uses tcp for accaptence, servers tcp newConnection to client is handeled in the remotes
			receiveTCP.request_stop();
			receiveTCP.join();
		}
		receiveTCP = std::jthread([this](std::stop_token s) { this->acceptNewClients(s); });
		WindowInstance.SetWindowsTitle("SERVER");
	}

	if(status == NetworkConnection::Status::initializedAsClient)
	{
		IsServer = false;
		HasStateAuthority = false;
		HasInputAuthority = true;

		if(receiveTCP.joinable())
		{ // Client listens in on tcp on this thread
			receiveTCP.request_stop();
			receiveTCP.join();
		}
		receiveTCP = std::jthread([this](std::stop_token s)
								  {
									  this->collectReceivedMessages(s,NetworkConnection::Protocol::TCP);
								  });


		HandshakeMessage message(runnerID);
		message.SetMessage(runnerName);
		Send(message,NetworkConnection::Protocol::TCP);
		WindowInstance.SetWindowsTitle("CLIENT");
	}


	if(Initialized)
	{
		//Both server and client should listen to UDP
		if(receiveUDP.joinable())
		{
			receiveUDP.request_stop();
			receiveUDP.join();
		}

		receiveUDP = std::jthread([this](std::stop_token s)
								  {
									  this->collectReceivedMessages(s,NetworkConnection::Protocol::UDP);
								  });
	}

	return status;
}

void NetworkRunner::Update()
{
	//if(!Initialized) {
	//	SessionConfiguration c;
	//	c.gameMode = SessionConfiguration::GameMode::Client;
	//	StartSession(c);
	//	return;
	//}

	moveMessageMapToRead();

	if(!IsServer) { return; }

	for(auto& [type,messagesToProcess] : messagesMap)
	{
		for(auto& incomingMessage : messagesToProcess)
		{
			ProcessIncoming(type,incomingMessage);
		}
	}
}

void NetworkRunner::ProcessIncoming(const eNetMessageType& type,NetworkRunner::RecievedMessage& incomingMessage)
{
	switch(type)
	{
	case eNetMessageType::ChatMessage:
	{
		const auto          message = std::bit_cast<ChatMessage>(incomingMessage.message);
		ChatMessage outMessage(message.GetId());
		outMessage.SetMessage(NameOfConnection(message.GetId()) + ": " + message.ReadMessage());
		incomingMessage.message = static_cast<NetMessage>(outMessage);
		Broadcast(outMessage,NetworkConnection::Protocol::TCP);
	}
	break;
	case eNetMessageType::NewConnection:
	{
		const auto          message = std::bit_cast<HandshakeMessage>(incomingMessage.message);
 		if(incomingMessage.idFrom == runnerID)
		{
			runnerName = message.ReadMessage();
			break;
		}

		if(auto* remote = IdToRemote(incomingMessage.idFrom))
		{
			remote->nickname = message.ReadMessage();
		}

		ChatMessage outMessage(message.GetId());
		outMessage.SetMessage(NameOfConnection(message.GetId()) + " Joined!");
		Broadcast(outMessage,NetworkConnection::Protocol::TCP);

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
		if(incomingMessage.idFrom == runnerID)
		{
			runnerName = message.ReadMessage();
			break;
		}

		if(auto* remote = IdToRemote(incomingMessage.idFrom))
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
		Broadcast(outMessage,NetworkConnection::Protocol::TCP);

		auto* remote = IdToRemote(incomingMessage.idFrom);
		if(remote != nullptr)
		{
			remote->Close();
		}
	}
	break;
	case eNetMessageType::PlayerJoin:
		break;
	case eNetMessageType::TransformSyncMessage:
	{
		const auto          message = std::bit_cast<PlayerSyncMessage>(incomingMessage.message);
		PlayerSyncData data = message.ReadMessage();

		// std::cout << data.position << '\n' << data.rotation << '\n' << data.scale << '\n';

		//for(const auto& [id,name] : usersNames) {
		//	if(id == message->GetId()) {
		//		continue;
		//	}

		//	PlayerSyncMessage outMessage;
		//	outMessage.SetMessage(data);
		//	//newConnection.Send(outMessage,usersIPs[id]);
		//}
	}
	break;
	case eNetMessageType::Count:
		break;
	default:
		//For all not custom proccessed messages, sendem to everyone
		Broadcast(incomingMessage.message,incomingMessage.protocol);
		break;
	}
}

void NetworkRunner::Close()
{

	for(auto& remote : remoteConnections)
	{
		remote.Close();
	}


	receiveTCP.request_stop();
	receiveUDP.request_stop();
	connection.Close();
}

bool NetworkRunner::Send(NetMessage& message,NetworkConnection::Protocol protocol)
{
	if(!Initialized) { return false; }
	message.Id = runnerID;

	if(IsServer)
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

	//Connection is bound to server
	return connection.Send(message,protocol);
}

bool NetworkRunner::HasConnection(NetworkedId id)
{

	if(id == runnerID && connection.GetStatus() == NetworkConnection::Status::initialized)
	{
		return true;
	}

	if(IsServer)
	{
		for(const auto& remote : remoteConnections)
		{
			if(remote.isConnected && remote.id == id)
			{
				return true;
			}
		}
	}

	return false;
}

std::string NetworkRunner::NameOfConnection(NetworkedId id)
{
	if(id == runnerID && (connection.GetStatus() && NetworkConnection::Status::initialized))
	{
		return runnerName;
	}

	const Remote* remote = IdToRemote(id);
	if(remote != nullptr)
	{
		return remote->nickname;
	}

	return "";
}

Remote* NetworkRunner::IdToRemote(NetworkedId id)
{
	if(id == runnerID)
	{
		return nullptr;
	}

	if(IsServer)
	{
		for(auto& remote : remoteConnections)
		{
			if(remote.isConnected && remote.id == id)
			{
				return &remote;
			}
		}
	}
	return nullptr;
}

bool NetworkRunner::Broadcast(const NetMessage& message,NetworkConnection::Protocol protocol)
{
	assert(IsServer);
	bool allSucceded = true;
	for(const auto& client : remoteConnections)
	{
		if(!client.isConnected) { continue; }
		allSucceded &= client.remoteConnection.Send(message,client.remoteConnection.Address(),protocol);
	}

	return allSucceded;
}

bool NetworkRunner::Unicast(const NetMessage& message,Remote client,NetworkConnection::Protocol protocol)
{
	return connection.Send(message,client.remoteConnection.Address(),protocol);
}

bool NetworkRunner::Multicast(const NetMessage& message,std::span<Remote> spanOfClient,NetworkConnection::Protocol protocol)
{
	bool allSucceded = true;
	for(const auto& client : spanOfClient)
	{
		allSucceded &= client.remoteConnection.Send(message,client.remoteConnection.Address(),protocol);
	}

	return allSucceded;
}

void NetworkRunner::acceptNewClients(std::stop_token stop_token)
{
	NetworkConnection newConnection;
	while(!stop_token.stop_requested())
	{
		if(connection.Accept(&newConnection))
		{
			for(auto& client : remoteConnections)
			{
				if(client.isConnected == false)
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

void NetworkRunner::collectReceivedMessages(std::stop_token stop_token,NetworkConnection::Protocol protocol)
{
	if(!Initialized) { return; }

	NetMessage incomingMessage;
	NetAddress recievedFromAddress;
	while(!stop_token.stop_requested())
	{
		if(protocol == NetworkConnection::Protocol::UDP)
		{
			if(connection.ReceiveUDP(&incomingMessage,&recievedFromAddress))
			{
				std::scoped_lock lock(addMessageLock);

				RecievedMessage recv{
					incomingMessage,
					recievedFromAddress,
					incomingMessage.GetId()
				};

				threadedMessageMap[incomingMessage.myType].emplace_back(recv);
			}
		}

		if(protocol == NetworkConnection::Protocol::TCP)
		{
			if(connection.ReceiveTCP(&incomingMessage,&recievedFromAddress))
			{
				std::scoped_lock lock(addMessageLock);

				RecievedMessage recv{
					incomingMessage,
					recievedFromAddress,
					incomingMessage.GetId()
				};

				threadedMessageMap[incomingMessage.myType].emplace_back(recv);
			}
		}
	}
}

void NetworkRunner::moveMessageMapToRead()
{
	std::scoped_lock lock(addMessageLock);

	for(auto& [type,messageVector] : messagesMap)
	{
		messageVector.clear();
	}

	for(auto& [type,messageVector] : threadedMessageMap)
	{
		messagesMap[type] = messageVector;
		messageVector.clear();
	}

	for(auto& remote : remoteConnections)
	{
		if(remote.isConnected && !remote.messages.empty())
		{
			for(const auto& message : remote.Read())
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

#pragma optimize( "", on )
