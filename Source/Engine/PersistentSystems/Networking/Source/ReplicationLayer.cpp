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
#include <span>
#include <ratio>
#include <chrono>
#include "Engine\AssetManager\ComponentSystem\Components\Collider.h"

ReplicationLayer::ReplicationLayer() : spacialFrequencyCulling(0, 0, 100.0f)
{
}

//Here you fucker, begin here. Make sure the AOI is registed not transported
void ReplicationLayer::fixedNetworkUpdate(NetworkRunner& runner)
{
	OPTICK_EVENT();

	if (runner.IsServer)
	{
		server_ReadIncoming(runner);
		server_fixedNetworkUpdate(runner);
	}
	else
	{
		client_ReadIncoming(runner);
		client_fixedNetworkUpdate(runner);
	}
	spacialFrequencyCulling.Draw(GetRenderer().debugDrawer);
}
void ReplicationLayer::server_ReadIncoming(NetworkRunner& runner)
{
	if (runner.messagesMap.contains(RegisterPlayerMessage::type))
	{
		for (auto& message : runner.messagesMap.at(RegisterPlayerMessage::type))
		{
			auto msg = std::bit_cast<RegisterPlayerMessage>(message.message);
			auto remoteAoI = msg.ReadMessage();

			if (idToObjectMap.contains(remoteAoI.owner))
			{
				Remote* remote = runner.IdToRemote(msg.GetId());
				if (remote)
				{
					remote->areaOfInterest = remoteAoI;
				}
			}
		}
	}
	if (runner.messagesMap.contains(InputEventMessage::type))
	{
		for (auto& message : runner.messagesMap.at(InputEventMessage::type))
		{
			auto msg = std::bit_cast<InputEventMessage>(message.message);
			auto inputEvent = msg.ReadMessage();
			 
		}
	}
}

void ReplicationLayer::server_fixedNetworkUpdate(NetworkRunner& runner)
{
	OPTICK_EVENT();
	for (auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		auto activeConnections = runner.remoteConnections | std::ranges::views::filter([&](auto&& x)
		{
			const auto syncRate = ShouldSpacialCull(networkedTransform.myPosition, x.GetAreaOfInterest());
			return x.isConnected
				&& (networkedTransform.neverNotSync || syncRate != Networking::SyncRates::noUpdate)
				&& networkedTransform.ShouldSync(runner, syncTimes[Cast<int>(std::clamp(syncRate, Networking::SyncRates::low, Networking::SyncRates::high))]);
		});

		auto& transform = networkedTransform.transform();
		//std::ranges::for_each(activeConnections, [&](auto&& x)
		//{
		//	auto& connectionCuller = spacialFrequencyCulling.GetDataForObject(x.GetId());
		//	connectionCuller.id = x.GetId();
		//	const auto syncRate = ShouldSpacialCull(networkedTransform.myPosition, x.GetAreaOfInterest());
		//	connectionCuller.frequencyInverse = syncTimes[Cast<int>(std::clamp(syncRate, Networking::SyncRates::low, Networking::SyncRates::high))];
		//});
		//if(!transform.GetIsDirty()) {
		//	continue;
		//}

		networkedTransform.myPosition = transform.myPosition;
		networkedTransform.myQuaternion = transform.myQuaternion;
		networkedTransform.myScale = transform.myScale;

		TransformSyncData data{
			networkedTransform.GetServerID(),
			networkedTransform.myPosition,
			networkedTransform.myQuaternion,
			networkedTransform.myScale
		};

		TransformSyncMessage composedTransformUpdate;
		composedTransformUpdate.SetMessage(data);
		runner.Multicast(composedTransformUpdate, activeConnections, NetworkConnection::Protocol::UDP);
	}
}

void ReplicationLayer::client_fixedNetworkUpdate(const NetworkRunner& runner) const
{
	OPTICK_EVENT();

	runner;
	auto now = runner.serverTime();

	//Check up to date idToObjectMap
	for (const auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		if (!idToObjectMap.contains(networkedTransform.GetServerID()))
		{
			Scene::activeManager().DeleteGameObject(networkedTransform.gameObject());
			continue;
		}
	}

	//Interpolationsystem
	for (auto& networkedTransform : Scene::activeManager().GetAllComponents<NetworkTransform>())
	{
		auto& transform = networkedTransform.transform();

		//networkedTransform.translationInterpolation is in unit(meter) per second
		std::chrono::duration<float> timeDifference = now - networkedTransform.GetLastSyncTime(); // Time
		const auto delta = timeDifference.count();
		//networkedTransform.translationInterpolation = networkedTransform.myPosition - transform.myPosition; // Distance

		//Distance/Time = velocity
		LOGGER.LogC("Delta at fixed update", delta);
		auto newPosition = networkedTransform.myPosition + networkedTransform.translationInterpolation * delta;
		//auto newQuaternion = figure this fucker out sometime * secondsDifference; 
		transform.SetPosition(newPosition);
		transform.SetQuatF(networkedTransform.myQuaternion);
		transform.SetScale(networkedTransform.myScale);
	}
}

void ReplicationLayer::client_ReadIncoming(const NetworkRunner& runner)
{
	OPTICK_EVENT();

	if (runner.messagesMap.contains(DestroyObjectMessage::type))
	{
		for (auto& message : runner.messagesMap.at(DestroyObjectMessage::type))
		{
			auto msg = std::bit_cast<DestroyObjectMessage>(message.message);
			auto id = msg.ReadMessage();

			if (idToObjectMap.contains(id))
			{
				Scene::activeManager().DeleteGameObject(idToObjectMap.at(id).gameObject());
				idToObjectMap.erase(id);
			}
		}
	}
	if (runner.messagesMap.contains(RegisterPlayerMessage::type))
	{
		for (auto& message : runner.messagesMap.at(RegisterPlayerMessage::type))
		{
			auto msg = std::bit_cast<RegisterPlayerMessage>(message.message);
			auto messageContent = msg.ReadMessage();

			if (!idToObjectMap.contains(messageContent.owner))
			{
				GameObject player = GameObject::Create("Player");
				auto& renderer = player.AddComponent<MeshRenderer>("Models/C64.fbx");
				player.transform().SetPosition(Vector3f::zero());

				auto& collider = player.AddComponent<Collider>();
				player.AddComponent<NetworkObject>(messageContent.owner);
				auto& listener = player.AddComponent<NetworkInputListener>();
				auto object = player.AddComponent<NetworkTransform>();
				object.RegisterAsPlayer();
				collider.SetColliderType<ColliderAssetSphere>();

				if (const auto mat = Resources.ForceLoad<Material>("TreeMaterial"))
				{
					mat->SetColor(ColorManagerInstance.GetColor("Blue"));
					renderer.SetMaterial(mat);
				}

				listener.AddKeyFunc(Keys::W, Action::MoveCharacter, false, [&](InputContext context)
				{
					if (context.state.phase == Phase::Pressed || context.state.phase == Phase::Held)
					{
						auto object = Scene::activeManager().GetGameObject((context.id));
						object.transform().Move(Vector3f::forward() * TimerInstance.getDeltaTime() * 20.f);
						return InputResponse::claimInput;
					}
					return InputResponse::undecided;
				});
				listener.AddKeyFunc(Keys::S, Action::MoveCharacter, false, [&](InputContext context)
				{
					if (context.state.phase == Phase::Pressed || context.state.phase == Phase::Held)
					{
						auto object = Scene::activeManager().GetGameObject((context.id));
						object.transform().Move(-Vector3f::forward() * TimerInstance.getDeltaTime() * 20.f);
						return InputResponse::claimInput;
					}
					return InputResponse::undecided;
				});
				listener.AddKeyFunc(Keys::D, Action::MoveCharacter, false, [&](InputContext context)
				{
					if (context.state.phase == Phase::Pressed || context.state.phase == Phase::Held)
					{
						auto object = Scene::activeManager().GetGameObject((context.id));
						object.transform().Move(Vector3f::right() * TimerInstance.getDeltaTime() * 20.f);
						return InputResponse::claimInput;
					}
					return InputResponse::undecided;
				});
				listener.AddKeyFunc(Keys::A, Action::MoveCharacter, false, [&](InputContext context)
				{
					if (context.state.phase == Phase::Pressed || context.state.phase == Phase::Held)
					{
						auto object = Scene::activeManager().GetGameObject((context.id));
						object.transform().Move(-Vector3f::right() * TimerInstance.getDeltaTime() * 20.f);
						return InputResponse::claimInput;
					}
					return InputResponse::undecided;
				});
			}
		}
	}

	if (runner.messagesMap.contains(CreateObjectMessage::type))
	{
		for (auto& message : runner.messagesMap.at(CreateObjectMessage::type))
		{
			auto msg = std::bit_cast<CreateObjectMessage>(message.message);
			auto messageContent = msg.ReadMessage();

			if (!idToObjectMap.contains(messageContent.uniqueComponentId))
			{
				GameObject ball = GameObject::Create("Ball");
				auto& renderer = ball.AddComponent<MeshRenderer>("Models/BallEradicationGame/Sphere.fbx");
				if (const auto mat = Resources.ForceLoad<Material>("TreeMaterial"))
				{
					mat->SetColor(ColorManagerInstance.GetColor("Red"));
					renderer.SetMaterial(mat);
				}

				ball.AddComponent<NetworkObject>(messageContent.uniqueComponentId);
				ball.AddComponent<NetworkTransform>();

				idToObjectMap.emplace(messageContent.uniqueComponentId, ball.GetComponent<NetworkObject>());
			}
		}
	}

	auto now = runner.serverTime();
	if (runner.messagesMap.contains(TransformSyncMessage::type))
	{
		for (auto& transformMessage : runner.messagesMap.at(TransformSyncMessage::type))
		{
			auto msg = std::bit_cast<TransformSyncMessage>(transformMessage.message);
			TransformSyncData messageContent = msg.ReadMessage();

			if (!idToObjectMap.contains(messageContent.uniqueComponentId))
			{
				LOGGER.Warn("Received transform for not yet created object");

				auto remote = runner.IdToRemote(transformMessage.idFrom);
				if (!remote) { continue; }

				//If something is fucky we request the server to send appropriate message to set us right, ex: if missing it send a create
				QueryObjectStatus queryMsg;

				NetworkObjectStatus queryData;
				queryData.uniqueNetworkObject = messageContent.uniqueComponentId;
				queryData.isSpawned = false; 

				queryMsg.SetMessage(queryData);

				runner.SendTo(remote, queryMsg, NetworkConnection::Protocol::TCP);
				continue;
			}

			auto& netTransform = idToObjectMap.at(messageContent.uniqueComponentId).GetComponent<NetworkTransform>();
			auto previousSyncTime = netTransform.GetLastSyncTime();
			netTransform.Synced(msg.TimeSent());

			//std::chrono::duration<float> timeDifference = now - netTransform.GetLastSyncTime(); // Time
			//const auto delta = timeDifference.count();

			const Vector3 previousPosition = netTransform.myPosition;
			netTransform.myPosition = messageContent.myPosition;
			netTransform.myQuaternion = messageContent.myQuaternion;
			netTransform.myScale = messageContent.myScale;
			auto dt = std::chrono::duration<float>(netTransform.GetLastSyncTime() - previousSyncTime).count();
			if (dt > 0.0f)
			{
				netTransform.translationInterpolation = (netTransform.myPosition - previousPosition) / dt;
			}
		}
	}
}

void ReplicationLayer::ReceiveMessage(const NetMessage&)
{

}

bool ReplicationLayer::RegisterObject(const NetworkRunner& runner, const NetworkObject& object)
{
	OPTICK_EVENT();

	assert(runner.IsServer);

	if (!idToObjectMap.contains(object.GetServerID()))
	{
		//All registered objects have been sent to the clients 
		GameobjectInformation data;

		data.uniqueComponentId = object.GetServerID();
		for (const auto& cmp : object.gameObject().GetAllComponents())
		{
			data.listOfComponentsNames.emplace_back(cmp->GetTypeInfo().Name());
		}

		CreateObjectMessage createObjectMessage;
		createObjectMessage.SetMessage(data);
		runner.Broadcast(createObjectMessage, NetworkConnection::Protocol::TCP);
		idToObjectMap.emplace(object.GetServerID(), object);

		auto position = new cullerPosition{
			 object.GetServerID(),
			  60.0f,
			  0.0f
		};
		auto gridObject = new TreeObject<cullerPosition>(position);
		gridObject->border = Border2D(
			Vector2f(object.transform().GetPosition().x, object.transform().GetPosition().z),
			Vector2f(1.0f, 1.0f));
		gridObject->color = ColorManagerInstance.GetColor("Green");
		spacialFrequencyCulling.AddObject(gridObject);
		return true;
	}

	return false;
}

bool ReplicationLayer::UnRegisterObject(const NetworkRunner& runner, const NetworkObject& object)
{
	OPTICK_EVENT();
	assert(runner.IsServer);

	if (idToObjectMap.contains(object.GetServerID()))
	{
		//All registered objects have been sent to the clients  
		DestroyObjectMessage createObjectMessage;
		createObjectMessage.SetMessage(object.GetServerID());
		runner.Broadcast(createObjectMessage, NetworkConnection::Protocol::TCP);
		idToObjectMap.erase(object.GetServerID());
		return true;
	}
	return false;
}

void ReplicationLayer::RegisterPlayerAOI(Networking::AreaOfInterest newAOI)
{
	aoi = newAOI;
}

bool ReplicationLayer::ContainsObject(const NetworkedId& id) const
{
	return idToObjectMap.contains(id);
}

bool ReplicationLayer::TryFetchNetworkObject(const NetworkedId& id, NetworkObject* object) const
{
	if (!ContainsObject(id))
	{
		object = nullptr;
		return false;
	}
	*object = idToObjectMap.at(id);
	return true;
}

Networking::AreaOfInterest ReplicationLayer::AOI() const
{
	return aoi;
}

void ReplicationLayer::Close()
{
	idToObjectMap.clear();
}

Networking::SyncRates ReplicationLayer::ShouldSpacialCull(Vector3f positionOfObject, Networking::AreaOfInterest remoteZone) const
{
	if (remoteZone.area.IsInside(positionOfObject))
	{
		return Networking::SyncRates::high;
	}
	else if (remoteZone.area.Expanded(remoteZone.area.GetRadius() * 4).IsInside(positionOfObject))
	{
		return Networking::SyncRates::low;
	}
	else
	{
		return Networking::SyncRates::noUpdate;
	}


}
