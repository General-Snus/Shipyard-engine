#include "PersistentSystems.pch.h" 
#include "../Remote.h"

#include <Tools/Logging/Logging.h>
#include <Tools/Utilities/Math.hpp>
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include <Networking/NetMessage/StringMessages.h>

//void Client::Setup() {
//	int attemptLimit = 0;
//	while(!SetupConnection() && attemptLimit < 5) {
//		LOGGER.Log("Failed to setup connection try again");
//		attemptLimit++;
//	}
//}
//
//void Client::Update() {
//	//unsigned long ul = 1;
//	//ioctlsocket(connection.socket(),FIONBIO,&ul);
//
//	hasRecievedMessage = connection.Receive(buffer,&serverAddress);
//
//
//	//  switch (buffer->myType)
//	// {
//	// case eNetMessageType::None:
//	//     Logger.Log("MessageType was none");
//	//     break;
//	// case eNetMessageType::ChatMessage: {
//	//     ChatMessage *message = reinterpret_cast<ChatMessage *>(&inbound);
//	// 
//	//     /*std::cout << message->ReadMessage() << std::endl;*/
//	//     EditorInstance.myChatMessages.emplace_back(message->ReadMessage());
//	// }
//	// break;
//	// case eNetMessageType::NewConnection: {
//	// }
//	// break;
//	//     // case NetMessageType::TransformSyncMessage: {
//	//     //     PlayerSyncMessage *message = reinterpret_cast<PlayerSyncMessage *>(&inbound);
//	//     //     PlayerSyncData data = message->ReadMessage();
//	//     //
//	//     //     if (GameObject *object = SceneManager::Get().GetCurrentScene()->GetGameObject(data.id))
//	//     //     {
//	//     //         // update position of object
//	//     //         object->GetTransform()->SetPosition(data.position);
//	//     //         object->GetTransform()->SetRotation(data.rotation);
//	//     //         object->GetTransform()->SetScale(data.scale);
//	//     //
//	//     //         if (const auto animator = object->GetComponent<Animator>())
//	//     //         {
//	//     //             animator->SetBlendPosition(data.blendPosition);
//	//     //         }
//	//     //     }
//	//     //     else
//	//     //     {
//	//     //         // create new object with id
//	//     //         GameObject newObject;
//	//     //         newObject.SetID(data.id);
//	//     //         auto mesh = std::make_shared<MeshRenderer>(newObject);
//	//     //         mesh->SetMesh(R"(Animations\SKs\SK_Player.fbx)");
//	//     //
//	//     //         std::shared_ptr<Material> mat = std::make_shared<Material>();
//	//     //         MaterialBufferData matData;
//	//     //
//	//     //         const float word1 = (data.id) & 0xff;
//	//     //         const float word2 = (data.id >> 8) & 0xff;
//	//     //         const float word3 = (data.id >> 16) & 0xff;
//	//     //
//	//     //         CU::Vector3f color(word1, word2, word3);
//	//     //         color.Normalize();
//	//     //         matData.AlbedoColor = CU::Vector4f(color.x, color.y, color.z, 1.f);
//	//     //         mat->SetMaterialData(matData);
//	//     //
//	//     //         std::shared_ptr<MaterialAsset> material = std::make_shared<MaterialAsset>(L"TestMaterial", L"");
//	//     //         material->Init(mat);
//	//     //         EngineResources.AddAsset(material);
//	//     //         mesh->SetMaterial(material, 0);
//	//     //         newObject.AddComponent(mesh);
//	//     //
//	//     //         auto animator = std::make_shared<Animator>(newObject);
//	//     //         newObject.AddComponent(animator);
//	//     //
//	//     //         SceneManager::Get().GetCurrentScene()->AddGameObject(newObject);
//	//     //         GameObject *objPtr = SceneManager::Get().GetCurrentScene()->GetGameObject(newObject.GetID());
//	//     //         objPtr->Init();
//	//     //         objPtr->Start();
//	//     //
//	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Idle.fbx", PlayerAnimations::Idle);
//	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Walk.fbx", PlayerAnimations::Walk, {1.f,
//	//     //         0.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_StartJump.fbx",
//	//     PlayerAnimations::Walk,
//	//     //                                {0.0f, 0.5f});
//	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Fall.fbx", PlayerAnimations::Fall, {-1.f,
//	//     //         0.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_Glide.fbx", PlayerAnimations::Glide,
//	//     //         {-1.f, 1.f}); animator->AddAnimation(L"Animations\\Player\\AN_Player_Land.fbx",
//	//     //         PlayerAnimations::Land, {-1.f, -1.f});
//	//     //         animator->AddAnimation(L"Animations\\Player\\AN_Player_Attack.fbx", PlayerAnimations::Attack,
//	//     {0.f,
//	//     //         -1.f}); animator->SetLooping(true); animator->SetCurrentAnimation(0);
//	//     //     }
//	//     // }
//	//     break;
//	// case NetMessageType::Count:
//	//     break;
//	// default:
//	//     Logger.Log("MessageType was undefined");
//	//     break;
//	//}
//}

void Remote::Close() { // todo
	receiveTCP.request_stop(); 
	remoteConnection.Close();
	isConnected = false;
	Consume();
}

const std::vector<Remote::RemoteRecievedMessage>& Remote::Read() {
	std::scoped_lock remoteLock(messageMutex);
	return messages;
}

void Remote::Consume() {
	std::scoped_lock remoteLock(messageMutex);
	messages.clear();
}

void Remote::collectReceivedMessages(std::stop_token stop_token) {
	NetMessage incomingMessage;
	NetAddress recievedFromAddress;
	while(!stop_token.stop_requested()) {
		if(remoteConnection.ReceiveTCP(&incomingMessage,&recievedFromAddress)) {
			std::scoped_lock lock(messageMutex);
			//bit discusting but here we go

			if(incomingMessage.myType == eNetMessageType::NewConnection)
			{
				HandshakeMessage message = std::bit_cast<HandshakeMessage>(incomingMessage);
				nickname = message.ReadMessage();
				id = message.GetId();
			}
			

			lastRecievedMessageTime = std::chrono::system_clock::now();
			RemoteRecievedMessage recv{
				incomingMessage,
				recievedFromAddress
			};

			messages.emplace_back(recv);
		}
	}
}
