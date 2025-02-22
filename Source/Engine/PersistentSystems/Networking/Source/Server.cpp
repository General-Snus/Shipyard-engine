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

NetworkRunner::NetworkRunner() = default;
NetworkRunner::~NetworkRunner() = default;

HRESULT NetworkRunner::StartSession(SessionConfiguration configuration) {

	auto gameMode = configuration.gameMode;
	switch(gameMode) {
	case SessionConfiguration::GameMode::Single:
		//return StartGameModeSinglePlayer(args);
	case SessionConfiguration::GameMode::Shared:
	case SessionConfiguration::GameMode::Server:
	case SessionConfiguration::GameMode::Host:
	case SessionConfiguration::GameMode::Client:
	case SessionConfiguration::GameMode::AutoHostOrClient:
		connection.Initialize();
	default:
		gameMode = (SessionConfiguration::GameMode)0;
		return E_FAIL;
	}



}

void NetworkRunner::setup() {
	WSADATA data;
	if(WSAStartup(MAKEWORD(2,2),&data) != 0) {
		std::cout << "Wsa startup fail" << "\n";
	}



	std::cout << "Init done" << "\n";
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

NetworkConnection::Status NetworkConnection::Initialize(int socketType,int socketProtocol,unsigned short bindType) {
	Socket = socket(bindType,socketType,socketProtocol); // SOCK_DGRAM = UDP

	if(Socket == INVALID_SOCKET) {
		std::cout << "Could not create socket" << "\n";
		status = Status::failed;
		return status;
	}

	sockaddr_in server{};
	server.sin_family =(ADDRESS_FAMILY) bindType;
	server.sin_port = homeAddress.port;
	server.sin_addr.S_un.S_addr = homeAddress.address;

	if(bind(Socket,(sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
		std::cout << "Could not bind" << "\n";
		status = Status::failed;
		return status;
	}

	status = Status::initialized;

	return status;
}

bool NetworkConnection::Send(const NetMessage& message,const NetAddress& sendTo) {
	sockaddr addr = sendTo.asAddress();
	int length = sizeof(addr);

	if(sendto(Socket,(char*)&message,sizeof(message),0,&addr,length) == SOCKET_ERROR) {
		throw std::exception("ShittyFuckingInternet");
	}
}

bool NetworkConnection::Close() {
	closesocket(Socket);
	WSACleanup();

	return false;
}

bool NetworkConnection::Receive(NetMessage* const message,NetAddress* recivedFrom,const int bufferSize,const float timeout) {
	UNREFERENCED_PARAMETER(timeout);

	//static_assert(bufferSize <= 512);
	assert(bufferSize <= 512);

	if(status != Status::failed) {
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(bufferSize > 512) {
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(message->GetBuffer() == nullptr || recivedFrom == nullptr) {
		throw	std::bad_alloc();
	}

	sockaddr_in other;
	int         length = sizeof(other);

	auto recievedLength = recvfrom(Socket,(char*)&message,bufferSize,0,reinterpret_cast<sockaddr*>(&other),&length);
	if(recievedLength == SOCKET_ERROR && recievedLength > MAX_NETMESSAGE_SIZE) {
		return false;
	} else {
		recivedFrom->address = other.sin_addr.S_un.S_addr;
		recivedFrom->port = other.sin_port;
		return true;
	}
}

NetAddress::NetAddress() {
	if(inet_pton(AF_INET,LOCALHOST,&address) != 1) {
		throw std::exception("What the fuck");
	}
	port = htons(DEFAULT_PORT);
}

sockaddr NetAddress::asAddress() const {
	sockaddr_in intermediate{};
	std::memset(&intermediate,0,sizeof(intermediate));
	intermediate.sin_family = AF_INET;
	intermediate.sin_port = port;
	intermediate.sin_addr.S_un.S_addr = address;
	return *(sockaddr*)&intermediate;
}
