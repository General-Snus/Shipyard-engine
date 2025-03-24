#include "AssetManager.pch.h"

#include "../NetworkSync.h"

NetworkObject::NetworkObject(const SY::UUID anOwnerId,GameObjectManager* aManager): Component(anOwnerId,aManager)
{}
NetworkObject::NetworkObject(const SY::UUID anOwnerId,GameObjectManager* aManager,NetworkedId id): Component(anOwnerId,aManager),uniqueNetId(id)
{}

void NetworkObject::Init()
{
	Runner.registerObject(*this);
}

void NetworkObject::Destroy()
{
	Runner.unRegisterObject(*this);
}

bool NetworkObject::InspectorView()
{
	if(!Component::InspectorView())
	{
		return false;
	}
	Reflect<NetworkObject>();
	ImGui::InputText("Unique id",uniqueNetId.id.String().data(),uniqueNetId.id.String().size(),ImGuiInputTextFlags_ReadOnly);
	return true;
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

NetworkTransform::NetworkTransform(const SY::UUID anOwnerId,GameObjectManager* aManager): Component(anOwnerId,aManager)
{}

bool NetworkTransform::InspectorView()
{
	if(!Component::InspectorView())
	{
		return false;
	}
	Reflect<NetworkTransform>();
	ImGui::InputText("Unique id",uniqueNetId.id.String().data(),uniqueNetId.id.String().size(),ImGuiInputTextFlags_ReadOnly);
	return true;
}

void NetworkTransform::Init()
{
	if(auto* netObject =TryGetComponent<NetworkObject>())
	{
		//Rather out of place here, need a good place for prerequisite checks
		this->uniqueNetId = netObject->GetServerID();
	}
}
