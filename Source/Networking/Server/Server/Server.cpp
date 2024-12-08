#define NOMINMAX
#include "Server.h"

#include <iostream>

#include "Networking/NetShared/NetMessage/ChatMessage.h"
#include "Networking/NetShared/NetMessage/HandshakeMessage.h"
#include "Networking/NetShared/NetMessage/PlayerSyncMessage.h"
#include "Networking/NetShared/NetMessage/QuitMessage.h"

#undef GetMessage
#undef GetMessageW

Server::Server(): mySocket(0)
{
}

Server::~Server() = default;

void Server::setup()
{
	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "Wsa startup fail" << "\n";
	}

	mySocket = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM = UDP

	if (mySocket == INVALID_SOCKET)
	{
		std::cout << "Could not create socket" << "\n";
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = DEFAULT_PORT;
	server.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(mySocket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Could not bind" << "\n";
	}

	std::cout << "Init done" << "\n";
}

void Server::update()
{
	sockaddr_in other;
	int         length = sizeof(other);

	while (true)
	{
		char inbound[512];

		if (recvfrom(mySocket, inbound, 512, 0, reinterpret_cast<sockaddr*>(&other), &length) == SOCKET_ERROR)
		{
			/*std::cout << "Failed to receive" << "\n";*/
			continue;
		}

		switch (static_cast<eNetMessageType>(inbound[0]))
		{
		case eNetMessageType::None:
			break;
		case eNetMessageType::ChatMessage:
			{
				auto          message = reinterpret_cast<ChatMessage*>(&inbound);
				unsigned char recieveID = message->GetId();
				std::string   userMessage = "User: " + std::to_string(recieveID) + " With name: " + usersNames[
						recieveID] +
					" Wrote: " + message->ReadMessage();
				std::cout << userMessage << "\n";

				// user dont print their own message and only recieves the server message?

				for (const auto& [id, name] : usersNames)
				{
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[recieveID] + ":" + message->ReadMessage());

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&usersIPs[id]),
					           length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << "\n";
					}
				}
			}
			break;
		case eNetMessageType::Handshake:
			{
				auto message = reinterpret_cast<HandshakeMessage*>(&inbound);

				std::string joinMessage = "User: ";
				joinMessage += message->ReadMessage();
				joinMessage += " Joined!";
				std::cout << joinMessage << "\n";

				usersNames[latestId] = message->ReadMessage();

				HandshakeMessage newMessage;
				newMessage.SetId(latestId);

				char outbound[MAX_NETMESSAGE_SIZE];
				memcpy(outbound, &newMessage, sizeof(ChatMessage));

				if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&other), length) ==
					SOCKET_ERROR)
				{
					std::cout << "Couldn't send handshake" << "\n";
				}

				usersIPs[latestId] = other;

				for (const auto& [id, name] : usersNames)
				{
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[latestId] + " Joined!");
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&usersIPs[id]),
					           length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << "\n";
					}
				}

				latestId++;
			}
			break;
		case eNetMessageType::Quit:
			{
				auto          message = reinterpret_cast<QuitMessage*>(&inbound);
				unsigned char recieveID = message->GetId();
				std::string   userMessage =
					"User: " + std::to_string(recieveID) + " With name: " + usersNames[recieveID] +
					" Left the server";
				std::cout << userMessage << "\n";

				for (const auto& [id, name] : usersNames)
				{
					ChatMessage outMessage;
					outMessage.SetMessage(usersNames[recieveID] + " Left!");

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(ChatMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&usersIPs[id]),
					           length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << "\n";
					}
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
				auto           message = reinterpret_cast<PlayerSyncMessage*>(&inbound);
				PlayerSyncData data = message->ReadMessage();

				// std::cout << data.position << '\n' << data.rotation << '\n' << data.scale << '\n';

				for (const auto& [id, name] : usersNames)
				{
					if (id == message->GetId())
					{
						continue;
					}

					PlayerSyncMessage outMessage;
					outMessage.SetMessage(data);

					char outbound[MAX_NETMESSAGE_SIZE];
					memcpy(outbound, &outMessage, sizeof(PlayerSyncMessage));

					if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&usersIPs[id]),
					           length) == SOCKET_ERROR)
					{
						std::cout << "Couldn't send to other clients" << "\n";
					}
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

void Server::close()
{
	closesocket(mySocket);
	WSACleanup();
}
