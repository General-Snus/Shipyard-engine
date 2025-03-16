#include "AssetManager.pch.h"

#include "../NetworkSync.h"

NetworkObject::NetworkObject(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{}

bool NetworkObject::InspectorView()
{
	if(!Component::InspectorView())
	{
		return false;
	}
	Reflect<NetworkObject>();
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

NetworkTransform::NetworkTransform(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{}

bool NetworkTransform::InspectorView()
{
	return false;
}
