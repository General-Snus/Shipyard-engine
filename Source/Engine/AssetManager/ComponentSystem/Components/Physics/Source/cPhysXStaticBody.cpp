#include "AssetManager.pch.h"
#include <Tools/ThirdParty/PhysX/physx/include/foundation/PxMat33.h>
#include <Tools/ThirdParty/PhysX/physx/include/PxPhysicsAPI.h>
#include "../cPhysXStaticBody.h"


physx::PxMat33 ConvertMatrix(const Matrix& myMatrix)
{
	physx::PxMat33 returnMatrix;
	returnMatrix.column0 = PxVec3(myMatrix(1,1),myMatrix(1,2),myMatrix(1,3));
	returnMatrix.column1 = PxVec3(myMatrix(2,1),myMatrix(2,2),myMatrix(2,3));
	returnMatrix.column2 = PxVec3(myMatrix(3,1),myMatrix(3,2),myMatrix(3,3));
	return returnMatrix;
};

cPhysXStaticBody::cPhysXStaticBody(const SY::UUID anOwnerID) : Component(anOwnerID)
{
}

using namespace physx;
void cPhysXStaticBody::Init()
{
	const auto& transform = GetComponent<Transform>();

	auto* world = Shipyard_PhysX::Get().GetPhysicsWorld();
	data = world->createRigidStatic(
		PxTransform(
			transform.GetPosition(),
			PxQuat(ConvertMatrix(transform.GetTransform())))
	);

	Shipyard_PhysX::Get().GetScene()->addActor(*data);

	UpdateFromCollider();
}

void cPhysXStaticBody::UpdateFromCollider()
{
	if (auto* collider = TryGetAddComponent<cCollider>())
	{
		auto type = collider->GetColliderType();
		switch (type)
		{
		case eColliderType::AABB:
		{
			auto aabb = collider->GetColliderAssetOfType<ColliderAssetAABB>();
			const auto& aabbData = aabb->GetAABB();
			PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(aabbData.GetXSize() / 2,aabbData.GetYSize() / 2,aabbData.GetZSize() / 2),*Shipyard_PhysX::Get().GetDefaultMaterial());
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
			PxRigidActorExt::createExclusiveShape(*data,PxConvexMeshGeometry(Shipyard_PhysX::CookMesh(convexData)),*Shipyard_PhysX::Get().GetDefaultMaterial());
			break;
		}
		default:
			PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(1.f,1.0f,1.0f),*Shipyard_PhysX::Get().GetDefaultMaterial());
			break;
		}
	}
	else
	{
		assert(false && "failed to add collider for some reason, sim will not be as it should plz fix");
	}
}

void cPhysXStaticBody::Update()
{
}

void cPhysXStaticBody::Render()
{
}

void cPhysXStaticBody::Destroy()
{
	if (data)
	{
		data->release();
	}
}

void cPhysXStaticBody::OnSiblingChanged(const std::type_info* SourceClass)
{
	if (SourceClass == &typeid(cCollider))
	{
		UpdateFromCollider();
	}
}
