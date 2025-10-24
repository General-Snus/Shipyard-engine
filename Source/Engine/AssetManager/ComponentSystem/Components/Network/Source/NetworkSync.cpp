#include "AssetManager.pch.h"

#include "../NetworkSync.h"
#include "Engine/PersistentSystems/Networking/NetworkRunner.h"
#include "Engine\PersistentSystems\Networking\NetMessage\PlayerSyncMessage.h"
#include "Tools\Utilities\Input\EnumKeys.h"

NetworkObject::NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
}
NetworkObject::NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager, NetworkedId id) : Component(anOwnerId, aManager), uniqueNetId(id)
{
}

void NetworkObject::Init()
{
	Runner.registerObject(*this);
	Synced(Runner.serverTime());
}

void NetworkObject::Destroy()
{
	Runner.unRegisterObject(*this);
}

void NetworkObject::RegisterAsPlayer() const
{
	if (Runner.IsServer)
	{
		return;
	}

	assert(myManager->GetPlayer() == gameObject());

	const auto& playerTransform = transform();

	auto aoi = Networking::AreaOfInterest
	{
		.area = Sphere<float>(playerTransform.GetPosition(), ReplicationLayer::defaultAOIRange * playerTransform.GetScale().Dot(Vector3f::one())),
		.owner = uniqueNetId
	};

	Runner.layer.RegisterPlayerAOI(aoi);
	RegisterPlayerMessage message;
	message.SetMessage(Runner.layer.AOI());
	Runner.Send(message, NetworkConnection::Protocol::TCP);
}

bool NetworkObject::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect();
	ImGui::InputText("Unique id", uniqueNetId.id.String().data(), uniqueNetId.id.String().size(), ImGuiInputTextFlags_ReadOnly);
	return true;
}

bool NetworkObject::ShouldSync(const NetworkRunner& runner) const
{
	return ShouldSync(runner, SyncFrequencyInverse());
}

bool NetworkObject::ShouldSync(const NetworkRunner& runner, float CustomSyncFrequency) const
{
	auto diff = clientUpdateTimePoint - runner.serverTime();
	float secondSinceLastUpdate = (float)std::chrono::duration_cast<std::chrono::microseconds>(diff).count() * .001f;
	return secondSinceLastUpdate < 1 / CustomSyncFrequency;
}

void NetworkObject::Synced(const ServerTimePoint& time)
{
	clientUpdateTimePoint = time;
}

void NetworkObject::DisperseNetMessage(const NetMessage& netMessageForIndividualobject)
{
	netMessageForIndividualobject;
	//if() transform messageas
	// Get transform and apply data
	// if destroy message, please kill yourself
		//Creata message is handeled by runner that creates only gameobject { transform,networkobject} and then 
		// sends component creation messages to object

}

NetworkTransform::NetworkTransform(const SY::UUID anOwnerId, GameObjectManager* aManager) : NetworkObject(anOwnerId, aManager)
{
}

bool NetworkTransform::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}Reflect();
	ImGui::InputText("Unique id", uniqueNetId.id.String().data(), uniqueNetId.id.String().size(), ImGuiInputTextFlags_ReadOnly);
	return true;
}

void NetworkTransform::Init()
{
	if (auto* netObject = TryGetComponent<NetworkObject>())
	{
		//Rather out of place here, need a good place for prerequisite checks 
		this->syncFrequency = netObject->syncFrequency;
		this->uniqueNetId = netObject->GetServerID();
		this->clientUpdateTimePoint = netObject->GetLastSyncTime();
	}
}

NetworkInputListener::NetworkInputListener(const SY::UUID anOwnerId, GameObjectManager* aManager) : NetworkObject(anOwnerId, aManager)
{
}

void NetworkInputListener::Init()
{
}

bool NetworkInputListener::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect();
	ImGui::InputText("Unique id", uniqueNetId.id.String().data(), uniqueNetId.id.String().size(), ImGuiInputTextFlags_ReadOnly);
	return true;
}

void NetworkInputListener::AddKeyFunc(Keys key, Action action, bool allowLocalPrediction, MapperFunction func)
{
	UNREFERENCED_PARAMETER(key);
	UNREFERENCED_PARAMETER(action);
	UNREFERENCED_PARAMETER(func);

	auto id = this->uniqueNetId;

	auto wrappedFunc = [id, allowLocalPrediction, func](InputContext c) ->InputResponse
	{
		if (Runner.IsServer)
		{
			return func(c);
		}

		InputEventMessage eventMessage;
		eventMessage.SetMessage(InputEventMessage::InputEventMessageData{ c.action, id });
		Runner.Send(eventMessage, NetworkConnection::Protocol::UDP);

		if (allowLocalPrediction)
		{
			return func(c);
		}
		return InputResponse::claimInput;
	};


	ServiceLocator::Instance().GetService<InputMapper>().AddListener(this->myOwnerID, key, action, wrappedFunc);
}
