#include "PersistentSystems.pch.h"
#include "../ReplicationLayer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Networking\NetMessage\PlayerSyncMessage.h"

void ReplicationLayer::fixedNetworkUpdate(NetworkRunner & runner)
{
	if(runner.IsServer)
	{
		server_fixedNetworkUpdate(runner);
	} else
	{
		client_fixedNetworkUpdate(runner);
	}
}

void ReplicationLayer::server_fixedNetworkUpdate(NetworkRunner & runner)
{
	auto now = std::chrono::steady_clock::now();
	for(auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		auto& transform = networkedTransform.transform(); 
		networkedTransform.myPosition = transform.myPosition;
		networkedTransform.myQuaternion= networkedTransform.myQuaternion;
		networkedTransform.myScale = networkedTransform.myScale;

		TransformSyncData data{
			networkedTransform.myPosition,
			networkedTransform.myQuaternion,
			networkedTransform.myScale
		};

		TransformSyncMessage composedTransformUpdate;
		composedTransformUpdate.SetMessage(data);
		runner.Broadcast(composedTransformUpdate,NetworkConnection::Protocol::UDP);

	}
}

void ReplicationLayer::client_fixedNetworkUpdate(const NetworkRunner & runner)
{
	auto now = std::chrono::steady_clock::now(); //switch out to server time
	for(auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		if(!idToObjectMap.contains(networkedTransform.GetServerID()))
		{
			Scene::activeManager().DeleteGameObject(networkedTransform.gameObject());
			continue;
		}

		auto& transform = networkedTransform.transform();

		//networkedTransform.translationInterpolation is in unit(meter) per second
		const auto timeDifference = now - networkedTransform.updatePoint;
		const auto secondsDifference = std::chrono::duration_cast<std::chrono::duration<float>>(timeDifference).count();

		auto newPosition = networkedTransform.myPosition + networkedTransform.translationInterpolation * secondsDifference;
		//auto newQuaternion = figure this fucker out sometime * secondsDifference; 

		transform.myPosition = newPosition;
		transform.myQuaternion= networkedTransform.myQuaternion;
		transform.myScale = networkedTransform.myScale;
	}

	//for(auto& i : Scene::activeManager().GetAllComponents<NetworkTransform>())
	//{
	//
	//}
	//
	//for(auto& i : Scene::activeManager().GetAllComponents<NetworkTransform>())
	//{
	//
	//}
}
