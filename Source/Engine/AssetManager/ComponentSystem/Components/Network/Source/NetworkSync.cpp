#include "AssetManager.pch.h"

#include "../NetworkSync.h"
#include "Engine/PersistentSystems/Networking/NetworkRunner.h"

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

void NetworkInputListener::AddKeyFunc(Keys key, Action action, MapperFunction func)
{
	UNREFERENCED_PARAMETER(key);
	UNREFERENCED_PARAMETER(action);
	UNREFERENCED_PARAMETER(func);

}
