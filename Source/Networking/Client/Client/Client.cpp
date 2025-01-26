#include "Client.h"

#include <string>
#include <thread>
#include <Tools/Logging/Logging.h>
#include <Tools/Utilities/Math.hpp>
#include "Networking/NetShared/NetMessage/ChatMessage.h"
#include "Networking/NetShared/NetMessage/HandshakeMessage.h"
#include "Networking/NetShared/NetMessage/NetMessage.h"
#include "Networking/NetShared/NetMessage/PlayerSyncMessage.h"
#include "Networking/NetShared/NetMessage/QuitMessage.h"

Client::Client() = default;
Client::~Client() = default;

void Client::Setup()
{
	int attemptLimit = 3;
	while (!SetupConnection() && attemptLimit > 0)
	{
		LOGGER.Log("Failed to setup connection try again");
		attemptLimit++;
	}
}

void Client::Update()
{
	unsigned long ul = 1;
	ioctlsocket(mySocket, FIONBIO, &ul);

	hasRecievedMessage =
		recvfrom(mySocket, inbound, MAX_NETMESSAGE_SIZE, 0, (sockaddr*)&myClient, &myLength) != SOCKET_ERROR;

	// switch (static_cast<NetMessageType>(inbound[0]))
	//{
	// case NetMessageType::None:
	//     Logger.Log("MessageType was none");
	//     break;
	// case NetMessageType::ChatMessage: {
	//     ChatMessage *message = reinterpret_cast<ChatMessage *>(&inbound);
	//
	//     /*std::cout << message->ReadMessage() << std::endl;*/
	//     EditorInstance.myChatMessages.emplace_back(message->ReadMessage());
	// }
	// break;
	// case NetMessageType::Handshake: {
	// }
	// break;
	//     // case NetMessageType::PlayerSync: {
	//     //     PlayerSyncMessage *message = reinterpret_cast<PlayerSyncMessage *>(&inbound);
	//     //     PlayerSyncData data = message->ReadMessage();
	//     //
	//     //     if (GameObject *object = SceneManager::Get().GetCurrentScene()->GetGameObject(data.id))
	//     //     {
	//     //         // update position of object
	//     //         object->GetTransform()->SetPosition(data.position);
	//     //         object->GetTransform()->SetRotation(data.rotation);
	//     //         object->GetTransform()->SetScale(data.scale);
	//     //
	//     //         if (const auto animator = object->GetComponent<Animator>())
	//     //         {
	//     //             animator->SetBlendPosition(data.blendPosition);
	//     //         }
	//     //     }
	//     //     else
	//     //     {
	//     //         // create new object with id
	//     //         GameObject newObject;
	//     //         newObject.SetID(data.id);
	//     //         auto mesh = std::make_shared<MeshRenderer>(newObject);
	//     //         mesh->SetMesh(R"(Animations\SKs\SK_Player.fbx)");
	//     //
	//     //         std::shared_ptr<Material> mat = std::make_shared<Material>();
	//     //         MaterialBufferData matData;
	//     //
	//     //         const float word1 = (data.id) & 0xff;
	//     //         const float word2 = (data.id >> 8) & 0xff;
	//     //         const float word3 = (data.id >> 16) & 0xff;
	//     //
	//     //         CU::Vector3f color(word1, word2, word3);
	//     //         color.Normalize();
	//     //         matData.AlbedoColor = CU::Vector4f(color.x, color.y, color.z, 1.f);
	//     //         mat->SetMaterialData(matData);
	//     //
	//     //         std::shared_ptr<MaterialAsset> material = std::make_shared<MaterialAsset>(L"TestMaterial", L"");
	//     //         material->Init(mat);
	//     //         EngineResources.AddAsset(material);
	//     //         mesh->SetMaterial(material, 0);
	//     //         newObject.AddComponent(mesh);
	//     //
	//     //         auto animator = std::make_shared<Animator>(newObject);
	//     //         newObject.AddComponent(animator);
	//     //
	//     //         SceneManager::Get().GetCurrentScene()->AddGameObject(newObject);
	//     //         GameObject *objPtr = SceneManager::Get().GetCurrentScene()->GetGameObject(newObject.GetID());
	//     //         objPtr->Init();
	//     //         objPtr->Start();
	//     //
	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Idle.fbx", PlayerAnimations::Idle);
	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Walk.fbx", PlayerAnimations::Walk, {1.f,
	//     //         0.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_StartJump.fbx",
	//     PlayerAnimations::Walk,
	//     //                                {0.0f, 0.5f});
	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Fall.fbx", PlayerAnimations::Fall, {-1.f,
	//     //         0.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_Glide.fbx", PlayerAnimations::Glide,
	//     //         {-1.f, 1.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_Land.fbx",
	//     //         PlayerAnimations::Land, {-1.f, -1.f});
	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Attack.fbx", PlayerAnimations::Attack,
	//     {0.f,
	//     //         -1.f}); animator->SetLooping(true); animator->SetCurrentAnimation(0);
	//     //     }
	//     // }
	//     break;
	// case NetMessageType::Count:
	//     break;
	// default:
	//     Logger.Log("MessageType was undefined");
	//     break;
	// }
}

void Client::Close()
{
	closesocket(mySocket);
	WSACleanup();

	sendThread.join();
	receiveThread.join();
}

bool Client::HasRecievedMessage()
{
	return hasRecievedMessage;
}

eNetMessageType Client::GetType()
{
	return static_cast<eNetMessageType>(inbound[0]);
}

char* Client::GetLatestMessage()
{
	return inbound;
}

bool Client::SetupConnection()
{
	WSADATA data;

	ZeroMemory(&mySocket, sizeof(mySocket));
	ZeroMemory(&myClient, sizeof(myClient));
	ZeroMemory(&myLength, sizeof(myLength));

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		LOGGER.Log("Wsa startup fail");
		return false;
	}

	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // SOCK_DGRAM = UDP

	if (mySocket == INVALID_SOCKET)
	{
		LOGGER.Log("Could not create socket");
		return false;
	}

	myClient.sin_family = AF_INET;

	short       port;
	std::string ip;
	// InputIpAndPort(ip, port);
	ip = DEFAULT_IP;
	port = DEFAULT_PORT;

	in_addr address;
	if (inet_pton(AF_INET, ip.c_str(), &address) != 1)
	{
		LOGGER.Log("Wrong IP");
		return false;
	}

	myClient.sin_addr = address;
	myClient.sin_port = port;

	myLength = sizeof(myClient);

	if (!SendOK())
	{
		LOGGER.Log("Didnt Get response from server");
		return false;
	}

	LOGGER.Log("Got ok from server");

	LOGGER.Log(".");
	LOGGER.Log(".");
	LOGGER.Log(".");

	LOGGER.Log(std::format("Connected with ip: {} \t | On Port: {} ", ip, port));
	LOGGER.Log("Init done");

	return true;
}

bool Client::SendOK()
{
	const std::string username = "User" + std::to_string(Math::RandomEngine::randomInRange(0, 999));

	HandshakeMessage message;
	message.SetMessage(username + "\n");

	char outbound[MAX_NETMESSAGE_SIZE];
	memcpy(outbound, &message, sizeof(HandshakeMessage));

	if (sendto(mySocket, outbound, 512, 0, reinterpret_cast<sockaddr*>(&myClient), sizeof(myClient)) == SOCKET_ERROR)
	{
		LOGGER.Log("Failed to send");
	}

	unsigned long ul = 1;
	ioctlsocket(mySocket, FIONBIO, &ul);

	ZeroMemory(&outbound, 64);

	int  iterations = 0;
	bool success = false;

	LOGGER.Log("Trying to connect");
	while (iterations < 100)
	{
		if (recvfrom(mySocket, outbound, 512, 0, (sockaddr*)&myClient, &myLength) != SOCKET_ERROR)
		{
			success = true;
			break;
		}
		iterations++;
	}

	if (success)
	{
		LOGGER.Log("Success recieving from server");
	}
	else
	{
		LOGGER.Log("Could not recieve from server");
	}
	const auto backMessage = reinterpret_cast<HandshakeMessage*>(&outbound);

	std::string joinMessage = "User: ";
	joinMessage += backMessage->ReadMessage();
	joinMessage += " Joined!";
	std::cout << joinMessage << std::endl;

	myId = backMessage->GetId();

	ul = 0;
	ioctlsocket(mySocket, FIONBIO, &ul);

	return success;
}

void Client::InputIpAndPort(std::string& outIp, short& outPort)
{
	std::cin.clear();
	LOGGER.Log("Input a Ip Adress (enter 0 for default 127.0.0.1) ");
	std::cin >> outIp;

	if (outIp == "0")
	{
		outIp = DEFAULT_IP;
	}

	std::cin.clear();

	LOGGER.Log("Input a port (enter 0 for default 27015) ");
	std::cin >> outPort;

	if (outPort == 0)
	{
		outPort = DEFAULT_PORT;
	}

	std::cin.clear();
}

void Client::MessageLoop()
{
}

void Client::Send(const std::string& aMessage)
{
	if (aMessage == "quit")
	{
		myExit = true;

		QuitMessage message;
		message.SetMessage("");
		message.SetId(myId);

		char outbound[MAX_NETMESSAGE_SIZE];
		memcpy(outbound, &message, sizeof(QuitMessage));

		if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myClient),
		           sizeof(myClient)) == SOCKET_ERROR)
		{
			LOGGER.Log("Failed to send");
		}
	}

	ChatMessage message;
	message.SetMessage(aMessage);
	message.SetId(myId);

	char outbound[MAX_NETMESSAGE_SIZE];
	memcpy(outbound, &message, sizeof(ChatMessage));

	const auto result =
		sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&myClient), sizeof(myClient));
	if (result == SOCKET_ERROR)
	{
		LOGGER.Log("Failed to send");
	}
}
