#include "AssetManager.pch.h"
#include "../cPhysXDynamicBody.h"
#include <Tools/ThirdParty/PhysX/physx/include/PxPhysicsAPI.h> 


using namespace physx;
cPhysXDynamicBody::cPhysXDynamicBody(const SY::UUID anOwnerID) : Component(anOwnerID)
{
}

void cPhysXDynamicBody::Init()
{
	const auto& transform = GetComponent<Transform>();

	auto* world = Shipyard_PhysX::Get().GetPhysicsWorld();
	data = world->createRigidDynamic(PxTransform(transform.GetPosition()));
	Shipyard_PhysX::Get().GetScene()->addActor(*data);

	PxMaterial* mMaterial;
	mMaterial = world->createMaterial(0.5f,0.5f,0.1f);
	if(!mMaterial)
	{
		assert(false && "Material creation failed!");
	}
	data->setLinearVelocity(PxVec3(0.f,10.f,0.f));
	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(1.f,1.0f,1.0f),*mMaterial);
}

void cPhysXDynamicBody::Update()
{
	const auto& pxTransform = data->getGlobalPose();

	Vector3f rotation;
	const Vector4f quat = {pxTransform.q.x,pxTransform.q.y,pxTransform.q.z,pxTransform.q.w};
	quaternion2Euler(quat,rotation);
	rotation *= RAD_TO_DEG;
	auto& transform = GetComponent<Transform>();

	const Vector3f position = {pxTransform.p.x,pxTransform.p.y,pxTransform.p.z};
	transform.SetPosition(position);
	transform.SetRotation(rotation);
}

void cPhysXDynamicBody::Render()
{
}
