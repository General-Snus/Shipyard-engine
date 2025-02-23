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

NetworkRunner::NetworkRunner() = default;
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
		break;
	case SessionConfiguration::GameMode::Shared:
		assert("Implement");
	case SessionConfiguration::GameMode::Server:
	case SessionConfiguration::GameMode::Host:
		status = connection.StartAsServer();
		if(status == NetworkConnection::Status::initialized) {
			IsServer = true;
			HasStateAuthority = true;
			HasInputAuthority = false;
		}
		break;
	case SessionConfiguration::GameMode::Client:
		status = connection.StartAsClient();
		if(status == NetworkConnection::Status::initialized) {
			IsServer = true;
			HasStateAuthority = true;
			HasInputAuthority = false;
		}
		break;
	case SessionConfiguration::GameMode::AutoHostOrClient:
		status = connection.AutoHostOrClient();
		if(status == NetworkConnection::Status::initialized) {
			IsServer = true;
			HasStateAuthority = true;
			HasInputAuthority = false;
		}
		break;
	default:
		status = NetworkConnection::Status::failed;
		break;
	}


	return status;
}



void NetworkRunner::update() {

	NetMessage incomingMessage;
	NetAddress recievedFromAddress;

	while(true) {
		if(!connection.Receive(&incomingMessage,&recievedFromAddress)) {
			continue;
		}

		switch(incomingMessage.myType) {
		case eNetMessageType::None:
			break;
		case eNetMessageType::ChatMessage:
		{
			auto          message = reinterpret_cast<ChatMessage*>(&incomingMessage);
			unsigned char recieveID = message->GetId();
			std::string   userMessage = "User: " + std::to_string(recieveID) + " With name: " + usersNames[
				recieveID] +
				" Wrote: " + message->ReadMessage();
				std::cout << userMessage << "\n";

				// user dont print their own message and only recieves the server message?

				for(const auto& [id,name] : usersNames) {
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[recieveID] + ":" + message->ReadMessage());
					connection.Send(outMessage,usersIPs[id]);
				}
		}
		break;
		case eNetMessageType::Handshake:
		{
			auto message = reinterpret_cast<HandshakeMessage*>(&incomingMessage);

			std::string joinMessage = "User: ";
			joinMessage += message->ReadMessage();
			joinMessage += " Joined!";
			std::cout << joinMessage << "\n";

			usersNames[latestId] = message->ReadMessage();

			HandshakeMessage newMessage;
			newMessage.SetId(latestId);

			connection.Send(newMessage,recievedFromAddress);
			usersIPs[latestId] = recievedFromAddress;

			for(const auto& [id,name] : usersNames) {
				ChatMessage outMessage;
				outMessage.SetMessage(usersNames[latestId] + " Joined!");
				connection.Send(outMessage,usersIPs[id]);
			}

			latestId++;
		}
		break;
		case eNetMessageType::Quit:
		{
			auto          message = reinterpret_cast<QuitMessage*>(&incomingMessage);
			unsigned char recieveID = message->GetId();
			const std::string   userMessage =
				"User: " + std::to_string(recieveID) + " With name: " + usersNames[recieveID] +
				" Left the server";

			for(const auto& [id,name] : usersNames) {
				ChatMessage outMessage;
				outMessage.SetMessage(usersNames[recieveID] + " Left!");
				connection.Send(outMessage,usersIPs[id]);
			}

			usersNames.erase(recieveID);
		}
		break;
		case eNetMessageType::PlayerJoin:
		{
		}
		break;
		case eNetMessageType::PlayerSync:
		{
			auto           message = reinterpret_cast<PlayerSyncMessage*>(&incomingMessage);
			PlayerSyncData data = message->ReadMessage();

			// std::cout << data.position << '\n' << data.rotation << '\n' << data.scale << '\n';

			for(const auto& [id,name] : usersNames) {
				if(id == message->GetId()) {
					continue;
				}

				PlayerSyncMessage outMessage;
				outMessage.SetMessage(data);
				connection.Send(outMessage,usersIPs[id]);
			}
		}
		break;
		case eNetMessageType::Count:
			break;
		default:
			break;
		}

		// char ip[17];
		// inet_ntop(AF_INET, &other.sin_addr, ip, 17);

		// std::cout << "Recieved Packet from: " << ip << "\n";

		// if (sendto(mySocket, inbound, 512, 0, reinterpret_cast<sockaddr*>(&other), length) == SOCKET_ERROR)
		//{
		//	std::cout << "Failed to respond back" << "\n";
		// }
	}
}

void NetworkRunner::close() {
	connection.Close();

}

void NetworkRunner::networkTransformUpdate() {}