#include "PersistentSystems.pch.h"
#include "../ReplicationLayer.h"
#include "../NetworkRunner.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Networking/NetMessage/PlayerSyncMessage.h"
#include "Networking/NetworkStructs.h"
#include "Networking/NetMessage/NetMessage.h"
#include "Engine/AssetManager/ComponentSystem/Components/Network/NetworkSync.h"

#include "Engine/AssetManager/GameResourcesLoader.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h" 
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h" 
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/PersistentSystems/Scene.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/Color.h"
#include "Tools\Utilities\Game\Timer.h"


#pragma optimize( "", off ) 
void ReplicationLayer::fixedNetworkUpdate(NetworkRunner & runner)
{
	OPTICK_EVENT();

	if(runner.IsServer)
	{
		server_fixedNetworkUpdate(runner);
	} else
	{
		client_ReadIncoming(runner);
		client_fixedNetworkUpdate(runner);
	}
}

void ReplicationLayer::server_fixedNetworkUpdate(NetworkRunner & runner)
{
	OPTICK_EVENT();

	auto now = std::chrono::steady_clock::now();
	for(auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		auto& transform = networkedTransform.transform();

		//if(!transform.GetIsDirty()) {
		//	continue;
		//}

		networkedTransform.myPosition = transform.myPosition;
		networkedTransform.myQuaternion= transform.myQuaternion;
		networkedTransform.myScale = transform.myScale;

		TransformSyncData data{
			networkedTransform.GetServerID(),
			networkedTransform.myPosition,
			networkedTransform.myQuaternion,
			networkedTransform.myScale
		};

		TransformSyncMessage composedTransformUpdate;
		composedTransformUpdate.SetMessage(data);
		runner.Broadcast(composedTransformUpdate,NetworkConnection::Protocol::UDP);
	}
}

void ReplicationLayer::client_fixedNetworkUpdate(const NetworkRunner & runner) const
{
	OPTICK_EVENT();

	runner;
	auto now = std::chrono::high_resolution_clock::now(); //switch out to server time

	//Check up to date idToObjectMap
	for(const auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		if(!idToObjectMap.contains(networkedTransform.GetServerID()))
		{
			Scene::activeManager().DeleteGameObject(networkedTransform.gameObject());
			continue;
		}
	}

	//Interpolationsystem
	for(auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		auto& transform = networkedTransform.transform();

		//networkedTransform.translationInterpolation is in unit(meter) per second
		const auto timeDifference = now - networkedTransform.updatePoint; // Time 
		networkedTransform.translationInterpolation = networkedTransform.myPosition - transform.myPosition; // Distance

		//Distance/Time = velocity
		const float delta = TimerInstance.getDeltaTime();
		auto newPosition = networkedTransform.myPosition + networkedTransform.translationInterpolation * delta;
		//auto newQuaternion = figure this fucker out sometime * secondsDifference; 
		transform.SetPosition(newPosition);
		transform.SetQuatF(networkedTransform.myQuaternion);
		transform.SetScale(networkedTransform.myScale);
	}
}

void ReplicationLayer::client_ReadIncoming(const NetworkRunner & runner)
{
	OPTICK_EVENT();

	if(runner.messagesMap.contains(DestroyObjectMessage::type))
	{
		for(auto& message : runner.messagesMap.at(DestroyObjectMessage::type))
		{
			auto msg =std::bit_cast<DestroyObjectMessage>(message.message);
			auto id = msg.ReadMessage();

			if(idToObjectMap.contains(id))
			{
				Scene::activeManager().DeleteGameObject(idToObjectMap.at(id).gameObject());
				idToObjectMap.erase(id);
			}
		}
	}

	if(runner.messagesMap.contains(CreateObjectMessage::type))
	{
		for(auto& message : runner.messagesMap.at(CreateObjectMessage::type))
		{
			auto msg =std::bit_cast<CreateObjectMessage>(message.message);
			auto messageContent = msg.ReadMessage();

			if(!idToObjectMap.contains(messageContent.uniqueComponentId))
			{
				GameObject ball = GameObject::Create("Ball");
				auto& renderer = ball.AddComponent<MeshRenderer>("Models/BallEradicationGame/Sphere.fbx");
				if(const auto mat = Resources.ForceLoad<Material>("TreeMaterial")) {
					mat->SetColor(ColorManagerInstance.GetColor("Red"));
					renderer.SetMaterial(mat);
				}

				ball.AddComponent<NetworkObject>(messageContent.uniqueComponentId);
				ball.AddComponent<NetworkTransform>();

				idToObjectMap.emplace(messageContent.uniqueComponentId,ball.GetComponent<NetworkObject>());
			}
		}
	}

	if(runner.messagesMap.contains(TransformSyncMessage::type))
	{
		for(auto& transformMessage : runner.messagesMap.at(TransformSyncMessage::type))
		{
			auto msg =std::bit_cast<TransformSyncMessage>(transformMessage.message);
			TransformSyncData messageContent = msg.ReadMessage();

			if(!idToObjectMap.contains(messageContent.uniqueComponentId))
			{
				LOGGER.Warn("Received transform for not yet created object");
				continue;
			}

			auto& netTransform = idToObjectMap.at(messageContent.uniqueComponentId).GetComponent<NetworkTransform>();

			netTransform.updatePoint = msg.TimeSent();
			netTransform.myPosition = messageContent.myPosition;
			netTransform.myQuaternion = messageContent.myQuaternion;
			netTransform.myScale = messageContent.myScale;
		}
	}


}

#pragma optimize( "", on )

void ReplicationLayer::receiveMessage(const NetMessage &)
{

}

bool ReplicationLayer::registerObject(const NetworkRunner & runner,const NetworkObject & object)
{
	OPTICK_EVENT();

	assert(runner.IsServer);

	if(!idToObjectMap.contains(object.GetServerID()))
	{
		//All registered objects have been sent to the clients 
		GameobjectInformation data;

		data.uniqueComponentId = object.GetServerID();
		for(const auto& cmp : object.gameObject().GetAllComponents())
		{
			data.listOfComponentsNames.emplace_back(cmp->GetTypeInfo().Name());
		}

		CreateObjectMessage createObjectMessage;
		createObjectMessage.SetMessage(data);
		runner.Broadcast(createObjectMessage,NetworkConnection::Protocol::TCP);
		idToObjectMap.emplace(object.GetServerID(),object);
		return true;
	}

	return false;
}

bool ReplicationLayer::unRegisterObject(const NetworkRunner & runner,const NetworkObject & object)
{
	OPTICK_EVENT();
	assert(runner.IsServer);

	if(idToObjectMap.contains(object.GetServerID()))
	{
		//All registered objects have been sent to the clients  
		DestroyObjectMessage createObjectMessage;
		createObjectMessage.SetMessage(object.GetServerID());
		runner.Broadcast(createObjectMessage,NetworkConnection::Protocol::TCP);
		idToObjectMap.erase(object.GetServerID());
		return true;
	}
	return false;
}

void ReplicationLayer::Close()
{
	idToObjectMap.clear();
}
