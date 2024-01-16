#include "AssetManager.pch.h"
#include <Tools/ThirdParty/PhysX/physx/include/PxPhysicsAPI.h> 
#include "../cPhysXDynamicBody.h"


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
	if (!mMaterial)
	{
		assert(false && "Material creation failed!");
	}

	if (auto* collider = TryGetAddComponent<cCollider>())
	{
		auto type = collider->GetColliderType();


		switch (type)
		{
		case eColliderType::AABB:
		{
			auto aabb = collider->GetColliderAssetOfType<ColliderAssetAABB>();
			const auto& aabbData = aabb->GetAABB();
			PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(aabbData.GetXSize(),aabbData.GetYSize(),aabbData.GetZSize()),*mMaterial);
			break;
		}
		//case eColliderType::SPHERE:
		//{
		//	auto* sphere = collider->TryGetAddComponent<ColliderAssetSphere>();
		//	const auto& sphereData = sphere->GetSphere();
		//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(sphereData.GetRadius()),*mMaterial);
		//	break;
		//}
		//case eColliderType::CIRCLE:
		//{
		//	auto* circle = collider->TryGetAddComponent<ColliderAssetCircle>();
		//	const auto& circleData = circle->GetCircle();
		//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(circleData.GetRadius()),*mMaterial);
		//	break;
		//}
		//case eColliderType::PLANE:
		//{
		//	auto* plane = collider->TryGetAddComponent<ColliderAssetPlane>();
		//	const auto& planeData = plane->GetPlane();
		//	PxRigidActorExt::createExclusiveShape(*data,PxPlaneGeometry(),*mMaterial);
		//	break;
		//}
		//case eColliderType::CONE:
		//{
		//	auto* cone = collider->TryGetAddComponent<ColliderAssetCone>();
		//	const auto& coneData = cone->GetCone();
		//	PxRigidActorExt::createExclusiveShape(*data,PxCapsuleGeometry(coneData.GetRadius(),coneData.GetHeight()),*mMaterial);
		//	break;
		//}
		//case eColliderType::FRUSTRUM:
		//{
		//	auto* frustrum = collider->TryGetAddComponent<ColliderAssetFrustrum>();
		//	const auto& frustrumData = frustrum->GetFrustrum();
		//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(frustrumData.GetWidth(),frustrumData.GetHeight(),frustrumData.GetDepth()),*mMaterial);
		//	break;
		//}
		//case eColliderType::TRIANGLE:
		//{
		//	auto* triangle = collider->TryGetAddComponent<ColliderAssetTriangle>();
		//	const auto& triangleData = triangle->GetTriangle();
		//	PxRigidActorExt::createExclusiveShape(*data,PxTriangleMeshGeometry(triangleData.GetMesh()),*mMaterial);
		//	break;
		//}
		//case eColliderType::RAY:
		//{
		//	auto* ray = collider->TryGetAddComponent<ColliderAssetRay>();
		//	const auto& rayData = ray->GetRay();
		//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(rayData.GetWidth(),rayData.GetHeight(),rayData.GetDepth()),*mMaterial);
		//	break;
		//}
		case eColliderType::CONVEX:
		{
			const auto convex = collider->GetColliderAssetOfType<ColliderAssetConvex>();
			const auto convexData = convex->GetColliderMesh();
			PxRigidActorExt::createExclusiveShape(*data,PxConvexMeshGeometry(Shipyard_PhysX::CookMesh(convexData)),*mMaterial);
			break;
		}
		default:
			PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(1.f,1.0f,1.0f),*mMaterial);
			break;
		}
	}
	else
	{
		assert(false && "failed to add collider for some reason, sim will not be as it should plz fix");
	}
}

void cPhysXDynamicBody::Update()
{
	const auto& pxTransform = data->getGlobalPose();

	Vector3f rotation;
	const Vector4f quat = { pxTransform.q.x,pxTransform.q.y,pxTransform.q.z,pxTransform.q.w };
	quaternion2Euler(quat,rotation);
	rotation *= RAD_TO_DEG;
	auto& transform = GetComponent<Transform>();

	const Vector3f position = { pxTransform.p.x,pxTransform.p.y,pxTransform.p.z };
	transform.SetPosition(position);
	transform.SetRotation(rotation);
}

void cPhysXDynamicBody::Render()
{
}

void cPhysXDynamicBody::Destroy()
{
	if (data)
	{
		data->release();
	}
}

void cPhysXDynamicBody::OnSiblingChanged(const std::type_info* SourceClass)
{
	if (SourceClass == &typeid(cCollider))
	{
		///UpdateFromCollider();
	}
}
