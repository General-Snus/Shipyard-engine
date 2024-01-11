#include "AssetManager.pch.h"
#include "../cPhysXDynamicBody.h"
#include <Tools/ThirdParty/PhysX/physx/include/PxActor.h>

cPhysXDynamicBody::cPhysXDynamicBody(const SY::UUID anOwnerID) : Component(anOwnerID)
{
	data = new TransformData();
}

cPhysXDynamicBody::cPhysXDynamicBody(const SY::UUID anOwnerID,TransformData* ID) : Component(anOwnerID)
{
	data = ID;
}

void cPhysXDynamicBody::SetId(TransformData* id)
{
	data = id;
}

TransformData* cPhysXDynamicBody::GetId()
{
	auto& transform = GetComponent<Transform>();

	data->position = transform.GetPosition();
	data->rotation = transform.GetRotation();
	return data;
}

void cPhysXDynamicBody::Init()
{
}

void cPhysXDynamicBody::Update()
{

	if(!data)
	{
		return;
	}
	auto& transform = GetComponent<Transform>();

	transform.SetPosition(data->position);
	transform.SetRotation(data->rotation);/*
	{
		const Vector3f position = *(Vector3f*)Shipyard_PhysX::Get().GetActor(bodyID)->userData;
		std::cout << "Position: " << position.x << " " << position.y << " " << position.z << std::endl;
	}*/
}

void cPhysXDynamicBody::Render()
{
}
