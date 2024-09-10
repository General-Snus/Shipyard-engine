#define NOMINMAX 
#include "Server.h"

#include <iostream>

#include "Networking/NetShared/NetMessage/ChatMessage.h"
#include "Networking/NetShared/NetMessage/HandshakeMessage.h"
#include "Networking/NetShared/NetMessage/PlayerSyncMessage.h"
#include "Networking/NetShared/NetMessage/QuitMessage.h" 

#undef GetMessage
#undef GetMessageW

Server::Server()
{
}

Server::~Server()
{
}

void Server::Setup()
{
	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "Wsa startup fail" << std::endl;
	}

	mySocket = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM = UDP

	if (mySocket == INVALID_SOCKET)
	{
		std::cout << "Could not create socket" << std::endl;
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = DEFAULT_PORT;
	server.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(mySocket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Could not bind" << std::endl;
	}

	std::cout << "Init done" << std::endl;

}

void Server::Update()
{
	sockaddr_in other;
	int length = sizeof(other);

	while (true)
	{
		char inbound[512];

		if (recvfrom(mySocket, inbound, 512, 0, reinterpret_cast<sockaddr*>(&other), &length) == SOCKET_ERROR)
		{
			/*std::cout << "Failed to receive" << std::endl;*/
			continue;
		}

		switch (static_cast<eNetMessageType>(inbound[0]))
		{
			case eNetMessageType::None:
				break;
			case eNetMessageType::ChatMessage:
			{
				ChatMessage* message = reinterpret_cast<ChatMessage*>(&inbound);
				unsigned char recieveID = message->GetId();
				std::string userMessage = "User: " + std::to_string(recieveID) + " With name: " + myUsersnames[recieveID] + " Wrote: " + message->ReadMessage();
				std::cout << userMessage << std::endl;

				//user dont print their own message and only recieves the server message?

				for (const auto& [id, name] : myUsersnames)
				{
					ChatMessage outMessage;
					outMessage.SetMessage(myUsersnames[recieveID] + ":" + message->ReadMessage());

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myUsersIPs[id]), length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << std::endl;
					}
				}
			}
			break;
			case eNetMessageType::Handshake:
			{
				HandshakeMessage* message = reinterpret_cast<HandshakeMessage*>(&inbound);

				std::string joinMessage = "User: ";
				joinMessage += message->ReadMessage();
				joinMessage += " Joined!";
				std::cout << joinMessage << std::endl;

				myUsersnames[myLatestId] = message->ReadMessage();

				HandshakeMessage newMessage;
				newMessage.SetId(myLatestId);

				char outbound[MAX_NETMESSAGE_SIZE];
				memcpy(outbound, &newMessage, sizeof(ChatMessage));

				if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&other), length) == SOCKET_ERROR)
				{
					std::cout << "Couldn't send handshake" << std::endl;
				}

				myUsersIPs[myLatestId] = other;

				for (const auto& [id, name] : myUsersnames)
				{
					ChatMessage outMessage;
					outMessage.SetMessage(myUsersnames[myLatestId] + " Joined!");

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myUsersIPs[id]), length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << std::endl;
					}
				}

				myLatestId++;
			}
			break;
			case eNetMessageType::Quit:
			{
				QuitMessage* message = reinterpret_cast<QuitMessage*>(&inbound);
				unsigned char recieveID = message->GetId();
				std::string userMessage = "User: " + std::to_string(recieveID) + " With name: " + myUsersnames[recieveID] + " Left the server";
				std::cout << userMessage << std::endl;

				for (const auto& [id, name] : myUsersnames)
				{
					

					ChatMessage outMessage;
					outMessage.SetMessage(myUsersnames[recieveID] + " Left!");

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myUsersIPs[id]), length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << std::endl;
					}
				}

				myUsersnames.erase(recieveID);
			}
			break;
			case eNetMessageType::PlayerJoin:
			{

			}
			break;
			case eNetMessageType::PlayerSync:
			{
				PlayerSyncMessage* message = reinterpret_cast<PlayerSyncMessage*>(&inbound);
				PlayerSyncData data = message->ReadMessage();

				//std::cout << data.position << '\n' << data.rotation << '\n' << data.scale << '\n';

				for (const auto& [id, name] : myUsersnames)
				{
					if (id == message->GetId())
					{
						continue;
					}

					PlayerSyncMessage outMessage;
					outMessage.SetMessage(data);

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(PlayerSyncMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myUsersIPs[id]), length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << std::endl;
					}
				}
			}
			break;
			case eNetMessageType::Count:
				break;
			default:
				break;
		}

		//char ip[17];
		//inet_ntop(AF_INET, &other.sin_addr, ip, 17);

		//std::cout << "Recieved Packet from: " << ip << std::endl;

		//if (sendto(mySocket, inbound, 512, 0, reinterpret_cast<sockaddr*>(&other), length) == SOCKET_ERROR)
		//{
		//	std::cout << "Failed to respond back" << std::endl;
		//}
	}
}

void Server::Close()
{
	closesocket(mySocket);
	WSACleanup();
}
