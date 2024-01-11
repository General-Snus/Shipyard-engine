#include "AssetManager.pch.h"
#include "../cPhysXDynamicBody.h"
#include <Tools/ThirdParty/PhysX/physx/include/PxPhysicsAPI.h> 
 
void threeaxisrot(float r11, float r12, float r21, float r31, float r32, float res[])
{
	res[0] = atan2(r31, r32);
	res[1] = asin(r21);
	res[2] = atan2(r11, r12);
}

void quaternion2Euler(const physx::PxQuat& q, float res[])
{
	threeaxisrot(-2 * (q.y * q.z - q.w * q.x),
		q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
		2 * (q.x * q.z + q.w * q.y),
		-2 * (q.x * q.y - q.w * q.z),
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
		res);
} 

using namespace physx;
cPhysXDynamicBody::cPhysXDynamicBody(const SY::UUID anOwnerID) : Component(anOwnerID)
{
} 

void cPhysXDynamicBody::Init()
{
	const auto& transform = GetComponent<Transform>();

	auto* world = Shipyard_PhysX::Get().GetPhysicsWorld();
	data = world->createRigidDynamic(PxTransform(transform.GetPosition()));

	PxMaterial* mMaterial;
	mMaterial = world->createMaterial(0.5f, 0.5f, 0.1f);
	if (!mMaterial)
	{
		assert(false && "Material creation failed!");
	}


	PxRigidActorExt::createExclusiveShape(*data, PxBoxGeometry(2.f, 0.2f, 0.1f), *mMaterial);
	PxRigidActorExt::createExclusiveShape(*data, PxBoxGeometry(0.2f, 2.f, 0.1f), *mMaterial);
	data->setAngularVelocity(PxVec3(0.f, 0.f, 5.f));
	data->setAngularDamping(0.f);
}

void cPhysXDynamicBody::Update()
{ 
	const auto& pxTransform = data->getGlobalPose();
	const Vector3f position = { pxTransform.p.x,pxTransform.p.y,pxTransform.p.z };

	float arr[3];
	quaternion2Euler(pxTransform.q, arr);
	const Vector3f rotation = { arr[0] * RAD_TO_DEG,arr[1] * RAD_TO_DEG,arr[2] * RAD_TO_DEG };

	auto& transform = GetComponent<Transform>();
	transform.SetPosition(position);
	transform.SetRotation(rotation);
}

void cPhysXDynamicBody::Render()
{
}
