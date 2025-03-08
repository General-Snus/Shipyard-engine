
#define NOMINMAX
#include "PersistentSystems.pch.h"


#include "../Server.h" 
#include "Engine/PersistentSystems/Networking/NetMessage/ChatMessage.h"
#include "Engine/PersistentSystems/Networking/NetMessage/HandshakeMessage.h"
#include "Engine/PersistentSystems/Networking/NetMessage/PlayerSyncMessage.h"
#include "Engine/PersistentSystems/Networking/NetMessage/QuitMessage.h"
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"

#undef GetMessage
#undef GetMessageW

#pragma optimize( "", off ) 
bool InitializeWinsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2),&wsaData);
	if(result == 0) {
		return true;
	}
	return false;
}

void CleanupWinsock() {
	WSACleanup();
}

NetworkRunner::NetworkRunner() {};
NetworkRunner::~NetworkRunner() { connection.Close(); CleanupWinsock(); };

NetworkConnection::Status NetworkRunner::StartSession(SessionConfiguration configuration) {

	if(!InitializeWinsock()) {
		LOGGER.ErrTrace("Server init failed");
		return NetworkConnection::Status::failed;;
	}

	connection.Close();

	NetworkConnection::Status status = NetworkConnection::Status::failed;

	switch(configuration.gameMode) {
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

	if(status == NetworkConnection::Status::initializedAsServer) {
		IsServer = true;
		HasStateAuthority = true;
		HasInputAuthority = true;

		//Start acceptance thread
		if(acceptConnection.joinable()) {
			acceptConnection.request_stop();
			acceptConnection.join();
		}
		acceptConnection = std::jthread([this](std::stop_token s) { this->acceptNewClients(s); });
	}

	if(status == NetworkConnection::Status::initializedAsClient) {
		IsServer = false;
		HasStateAuthority = false;
		HasInputAuthority = true;
	}

	Initialized = (status && NetworkConnection::Status::initialized);

	if(Initialized) {
		if(receiveTCP.joinable()) {
			receiveTCP.request_stop();
			receiveTCP.join();
		}
		if(receiveUDP.joinable()) {
			receiveUDP.request_stop();
			receiveUDP.join();
		}

		receiveTCP = std::jthread([this](std::stop_token s) {
			this->collectReceivedMessages(s,NetworkConnection::Protocol::TCP);
			});

		receiveUDP = std::jthread([this](std::stop_token s) {
			this->collectReceivedMessages(s,NetworkConnection::Protocol::UDP);
			});
	}

	return status;
}

void NetworkRunner::update() {
	if(!Initialized) {
		SessionConfiguration c;
		c.gameMode = SessionConfiguration::GameMode::Client;
		StartSession(c);
		return;
	}
	NetAddress recievedFromAddress;
	moveMessageMapToRead();
	


	for(const auto& [type,messagesToProcess] : messagesMap) {
		for(const auto& incomingMessage : messagesToProcess) {
			switch(type) {
			case eNetMessageType::None:
				break;
			case eNetMessageType::ChatMessage:
			{
				const auto          message = reinterpret_cast<const ChatMessage*>(&incomingMessage);
				unsigned char recieveID = message->GetId();
				std::string   userMessage = "User: " + std::to_string(recieveID) + " With name: " + usersNames[recieveID] +
					" Wrote: " + message->ReadMessage();
				std::cout << userMessage << "\n";

				// user dont print their own message and only recieves the server message? 
				for(const auto& [id,name] : usersNames) {
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[recieveID] + ":" + message->ReadMessage());
					//connection.Send(outMessage,usersIPs[id]);
				}
			}
			break;
			case eNetMessageType::Handshake:
			{
				const auto message = reinterpret_cast<const HandshakeMessage*>(&incomingMessage);

				std::string joinMessage = "User: ";
				joinMessage += message->ReadMessage();
				joinMessage += " Joined!";
				std::cout << joinMessage << "\n";

				usersNames[latestId] = message->ReadMessage();

				HandshakeMessage newMessage;
				newMessage.SetId(latestId);

				//connection.Send(newMessage,recievedFromAddress);
				usersIPs[latestId] = recievedFromAddress;

				for(const auto& [id,name] : usersNames) {
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[latestId] + " Joined!");
					//connection.Send(outMessage,usersIPs[id]);
				}

				latestId++;
			}
			break;
			case eNetMessageType::Quit:
			{
				auto          message = reinterpret_cast<const QuitMessage*>(&incomingMessage);
				unsigned char recieveID = message->GetId();
				const std::string   userMessage =
					"User: " + std::to_string(recieveID) + " With name: " + usersNames[recieveID] +
					" Left the server";

				for(const auto& [id,name] : usersNames) {
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[recieveID] + " Left!");
					//connection.Send(outMessage,usersIPs[id]);
				}

				usersNames.erase(recieveID);
			}
			break;
			case eNetMessageType::PlayerJoin:
				break;
			case eNetMessageType::PlayerSync:
			{
				const auto           message = reinterpret_cast<const PlayerSyncMessage*>(&incomingMessage);
				PlayerSyncData data = message->ReadMessage();

				// std::cout << data.position << '\n' << data.rotation << '\n' << data.scale << '\n';

				for(const auto& [id,name] : usersNames) {
					if(id == message->GetId()) {
						continue;
					}

					PlayerSyncMessage outMessage;
					outMessage.SetMessage(data);
					//connection.Send(outMessage,usersIPs[id]);
				}
			}
			break;
			case eNetMessageType::Count:
				break;
			default:
				break;
			}
		}
	}
}

void NetworkRunner::close() {
	connection.Close();  
	receiveTCP.request_stop();
	receiveUDP.request_stop();
	acceptConnection.request_stop();

}

bool NetworkRunner::Send(const NetMessage& message,NetworkConnection::Protocol protocol) {
	if(!Initialized) { return false; }

	if(IsServer) {
		Broadcast(message,protocol);
		std::scoped_lock lock(addMessageLock); 
		threadedMessageMap[message.myType].emplace_back(connection.Address(),message);
		return true;
	}

	//Connection is bound to server
	return connection.Send(message,protocol);
}

bool NetworkRunner::Broadcast(const NetMessage& message,NetworkConnection::Protocol protocol) {
	bool allSucceded = true;
	for(const auto& client : clients) { 
		allSucceded &= connection.Send(message,client.remoteConnection.Address(),protocol);
	}

	return allSucceded;
}

bool NetworkRunner::Unicast(const NetMessage& message,Remote client,NetworkConnection::Protocol protocol) {
	return connection.Send(message,client.remoteConnection.Address(),protocol);
}

bool NetworkRunner::Multicast(const NetMessage& message,std::span<Remote> spanOfClient,NetworkConnection::Protocol protocol) {
	bool allSucceded = true;
	for(const auto& client : spanOfClient) { 
		allSucceded &= connection.Send(message,client.remoteConnection.Address(),protocol);
	}

	return allSucceded;
}

void NetworkRunner::acceptNewClients(std::stop_token stop_token) {
	Remote remote;
	while(!stop_token.stop_requested()) {
		if(connection.Accept(&remote.remoteConnection)) {
			for(auto& client : clients) {
				if(client.isConnected == false) {
					client.Close();
					client.isConnected = true;
					client.remoteConnection = remote.remoteConnection;
					client.receiveTCP = std::jthread([&client](std::stop_token s) {
						client.collectReceivedMessages(s);
						});
					remote.remoteConnection = {};
					break;
				}
			}
		}
	}
}

void NetworkRunner::collectReceivedMessages(std::stop_token stop_token,NetworkConnection::Protocol protocol) {
	if(!Initialized) { return; }

	NetMessage incomingMessage;
	NetAddress recievedFromAddress;
	while(!stop_token.stop_requested()) {
		if(protocol == NetworkConnection::Protocol::UDP) {
			if(connection.ReceiveUDP(&incomingMessage,&recievedFromAddress)) {
				std::scoped_lock lock(addMessageLock);
				threadedMessageMap[incomingMessage.myType].emplace_back(recievedFromAddress,incomingMessage);
			}
		}

		if(protocol == NetworkConnection::Protocol::TCP) {
			if(connection.ReceiveTCP(&incomingMessage,&recievedFromAddress)) {
				std::scoped_lock lock(addMessageLock);
				threadedMessageMap[incomingMessage.myType].emplace_back(recievedFromAddress,incomingMessage);
			}
		}
	}
}

void NetworkRunner::moveMessageMapToRead() {
	std::scoped_lock lock(addMessageLock);
	for(auto& [type,messageVector] : threadedMessageMap) {
		messagesMap[type].clear();
		messagesMap[type].resize(messageVector.size());

		messagesMap[type] = messageVector;
		messageVector.clear();
	}
	
	for(auto& remote : clients) {
		if(remote.isConnected && !remote.messages.empty()) {
			for(const auto& message : remote.Read()) {
				messagesMap[message.myType].emplace_back(remote.remoteConnection.Address(),message);
			}
			remote.Consume();
		}
	}

}

#pragma optimize( "", on )
