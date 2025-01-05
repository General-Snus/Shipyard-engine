#include "AssetManager.pch.h"

#include "../cPhysXStaticBody.h"
#include <Tools/ThirdParty/PhysX/physx/include/PxPhysicsAPI.h>
#include <Tools/ThirdParty/PhysX/physx/include/foundation/PxMat33.h>
#include <extensions\PxRigidActorExt.h>

#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include "Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h"

physx::PxMat33 ConvertMatrix(const Matrix& myMatrix) {
	physx::PxMat33 returnMatrix;
	returnMatrix.column0 = physx::PxVec3(myMatrix(1,1),myMatrix(1,2),myMatrix(1,3));
	returnMatrix.column1 = physx::PxVec3(myMatrix(2,1),myMatrix(2,2),myMatrix(2,3));
	returnMatrix.column2 = physx::PxVec3(myMatrix(3,1),myMatrix(3,2),myMatrix(3,3));
	return returnMatrix;
};

cPhysXStaticBody::cPhysXStaticBody(const SY::UUID anOwnerId,GameObjectManager* aManager)
	: Component(anOwnerId,aManager) {}

using namespace physx;
void cPhysXStaticBody::Init() {
	auto const& transform = GetComponent<Transform>();
	auto& quat = transform.GetQuatF();
	auto* world = Shipyard_PhysXInstance.GetPhysicsWorld();
	data = world->createRigidStatic(PxTransform(transform.GetPosition(),PxQuat(quat.x,quat.y,quat.z,quat.w)));
	Shipyard_PhysXInstance.GetScene()->addActor(*data);

	data->setName(gameObject().GetName().c_str());
	data->userData = new SY::UUID(gameObject().GetID());


	UpdateFromCollider();
}

void cPhysXStaticBody::UpdateFromCollider() {
	OPTICK_EVENT();
	if(!data) {
		return;
	}

	auto& objectTransform = transform();
	if(const auto* collider = TryGetAddComponent<Collider>()) {
		if(!shape) {
			makeShape(collider,objectTransform);
		} else {
			updateShape(collider,objectTransform);
		}
	} else {
		assert(false && "failed to add collider for some reason, sim will not be as it should plz fix");
	}
}

void cPhysXStaticBody::makeShape(const Collider* collider,const Transform& transform) {
	switch(collider->GetColliderType()) {
	case eColliderType::BOX:
	{
		const auto aabb = collider->GetColliderAssetOfType<ColliderAssetBox>();
		//aabb->UpdateWithTransform(transform.unmodified_WorldMatrix());
		const auto& aabbData = aabb->box();
		auto geometry = PxBoxGeometry(aabbData.GetXSize() / 2,aabbData.GetYSize() / 2,aabbData.GetZSize() / 2);
		shape = Shipyard_PhysXInstance.CreateShape(data,&geometry);
		break;
	}
	case eColliderType::AABB:
	{
		const auto aabb = collider->GetColliderAssetOfType<ColliderAssetAABB>();
		//aabb->UpdateWithTransform(transform.unmodified_WorldMatrix());
		const auto& aabbData = aabb->ScaledAABB();
		auto geometry = PxBoxGeometry(aabbData.GetXSize() / 2,aabbData.GetYSize() / 2,aabbData.GetZSize() / 2);
		shape = Shipyard_PhysXInstance.CreateShape(data,&geometry);
		break;
	}
	// case eColliderType::SPHERE:
	//{
	//	auto* sphere = collider->TryGetAddComponent<ColliderAssetSphere>();
	//	const auto& sphereData = sphere->GetSphere();
	//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(sphereData.GetRadius()),*mMaterial);
	//	break;
	// }
	// case eColliderType::CIRCLE:
	//{
	//	auto* circle = collider->TryGetAddComponent<ColliderAssetCircle>();
	//	const auto& circleData = circle->GetCircle();
	//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(circleData.GetRadius()),*mMaterial);
	//	break;
	// }
	// case eColliderType::PLANE:
	//{
	//	auto* plane = collider->TryGetAddComponent<ColliderAssetPlane>();
	//	const auto& planeData = plane->GetPlane();
	//	PxRigidActorExt::createExclusiveShape(*data,PxPlaneGeometry(),*mMaterial);
	//	break;
	// }
	// case eColliderType::CONE:
	//{
	//	auto* cone = collider->TryGetAddComponent<ColliderAssetCone>();
	//	const auto& coneData = cone->GetCone();
	//	PxRigidActorExt::createExclusiveShape(*data,PxCapsuleGeometry(coneData.GetRadius(),coneData.GetHeight()),*mMaterial);
	//	break;
	// }
	// case eColliderType::FRUSTRUM:
	//{
	//	auto* frustrum = collider->TryGetAddComponent<ColliderAssetFrustrum>();
	//	const auto& frustrumData = frustrum->GetFrustrum();
	//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(frustrumData.GetWidth(),frustrumData.GetHeight(),frustrumData.GetDepth()),*mMaterial);
	//	break;
	// }
	// case eColliderType::TRIANGLE:
	//{
	//	auto* triangle = collider->TryGetAddComponent<ColliderAssetTriangle>();
	//	const auto& triangleData = triangle->GetTriangle();
	//	PxRigidActorExt::createExclusiveShape(*data,PxTriangleMeshGeometry(triangleData.GetMesh()),*mMaterial);
	//	break;
	// }
	// case eColliderType::RAY:
	//{
	//	auto* ray = collider->TryGetAddComponent<ColliderAssetRay>();
	//	const auto& rayData = ray->GetRay();
	//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(rayData.GetWidth(),rayData.GetHeight(),rayData.GetDepth()),*mMaterial);
	//	break;
	// }
	case eColliderType::CONVEX:
	{
		const auto convex = collider->GetColliderAssetOfType<ColliderAssetConvex>();
		const auto convexData = convex->GetColliderMesh();
		// PxRigidActorExt::createExclusiveShape(*data,PxConvexMeshGeometry(Shipyard_PhysX::CookMesh(convexData)),*Shipyard_PhysXInstance.GetDefaultMaterial());
		break;
	}
	case eColliderType::PLANAR:
	{
		// TODO: reenable planar mesh gen
		// const auto convex = collider->GetColliderAssetOfType<ColliderAssetPlanar>();
		// const auto convexData = convex->GetColliderMesh();
		// const Vector3f scaleV = transform.GetScale();
		// PxVec3 scale = { scaleV.x,scaleV.y,scaleV.z };
		// PxRigidActorExt::createExclusiveShape(*data,
		//	PxTriangleMeshGeometry(Shipyard_PhysX::CookMesh<physx::PxTriangleMesh>(convexData),scale),
		//	*Shipyard_PhysXInstance.GetDefaultMaterial());
		// break;
	}
	default:
		auto geometry = PxBoxGeometry(1,1,1);
		shape = Shipyard_PhysXInstance.CreateShape(data,&geometry);
		break;
	}

	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,true);

	if(collider->isTrigger) {
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE,false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE,true);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,false);
	}

	PxFilterData filterData;
	filterData.word0 = static_cast<unsigned>(gameObject().GetLayer());
	shape->setQueryFilterData(filterData);

	updateShape(collider,transform);
}

void cPhysXStaticBody::updateShape(const Collider* collider,const Transform& transform) {
	switch(collider->GetColliderType()) {
	case eColliderType::AABB:
	{
		const auto aabb = collider->GetColliderAssetOfType<ColliderAssetAABB>();
		aabb->UpdateWithTransform(transform.unmodified_WorldMatrix());
		const auto& aabbData = aabb->ScaledAABB();

		const auto xScale = 1.0f;//transform.unmodified_WorldMatrix().magnitudeOfRow(0);
		const auto yScale = 1.0f;//transform.unmodified_WorldMatrix().magnitudeOfRow(1);
		const auto zScale = 1.0f;//transform.unmodified_WorldMatrix().magnitudeOfRow(2); 

		auto geometry = PxBoxGeometry(xScale * aabbData.GetXSize() / 2,yScale * aabbData.GetYSize() / 2,zScale * aabbData.GetZSize() / 2);
		//const auto pxMatrix = PxMat44T<float>(*&transform.unmodified_WorldMatrix());
		//shape->setLocalPose(PxTransform(pxMatrix));
		shape->setGeometry(geometry);

		break;
	}
	case eColliderType::BOX:
	{
		const auto colliderAsset = collider->GetColliderAssetOfType<ColliderAssetBox>();
		colliderAsset->UpdateWithTransform(transform.unmodified_WorldMatrix());
		const auto& aabbData = colliderAsset->box();

		const auto scale = transform.unmodified_WorldMatrix().scale();
		const auto resizedBox = aabbData.GetExtent() * scale;
		const auto geometry = PxBoxGeometry(resizedBox);
		shape->setLocalPose(PxTransform(aabbData.GetCenter()));
		shape->setGeometry(geometry);

		break;
	}
	// case eColliderType::SPHERE:
	//{
	//	auto* sphere = collider->TryGetAddComponent<ColliderAssetSphere>();
	//	const auto& sphereData = sphere->GetSphere();
	//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(sphereData.GetRadius()),*mMaterial);
	//	break;
	// }
	// case eColliderType::CIRCLE:
	//{
	//	auto* circle = collider->TryGetAddComponent<ColliderAssetCircle>();
	//	const auto& circleData = circle->GetCircle();
	//	PxRigidActorExt::createExclusiveShape(*data,PxSphereGeometry(circleData.GetRadius()),*mMaterial);
	//	break;
	// }
	// case eColliderType::PLANE:
	//{
	//	auto* plane = collider->TryGetAddComponent<ColliderAssetPlane>();
	//	const auto& planeData = plane->GetPlane();
	//	PxRigidActorExt::createExclusiveShape(*data,PxPlaneGeometry(),*mMaterial);
	//	break;
	// }
	// case eColliderType::CONE:
	//{
	//	auto* cone = collider->TryGetAddComponent<ColliderAssetCone>();
	//	const auto& coneData = cone->GetCone();
	//	PxRigidActorExt::createExclusiveShape(*data,PxCapsuleGeometry(coneData.GetRadius(),coneData.GetHeight()),*mMaterial);
	//	break;
	// }
	// case eColliderType::FRUSTRUM:
	//{
	//	auto* frustrum = collider->TryGetAddComponent<ColliderAssetFrustrum>();
	//	const auto& frustrumData = frustrum->GetFrustrum();
	//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(frustrumData.GetWidth(),frustrumData.GetHeight(),frustrumData.GetDepth()),*mMaterial);
	//	break;
	// }
	// case eColliderType::TRIANGLE:
	//{
	//	auto* triangle = collider->TryGetAddComponent<ColliderAssetTriangle>();
	//	const auto& triangleData = triangle->GetTriangle();
	//	PxRigidActorExt::createExclusiveShape(*data,PxTriangleMeshGeometry(triangleData.GetMesh()),*mMaterial);
	//	break;
	// }
	// case eColliderType::RAY:
	//{
	//	auto* ray = collider->TryGetAddComponent<ColliderAssetRay>();
	//	const auto& rayData = ray->GetRay();
	//	PxRigidActorExt::createExclusiveShape(*data,PxBoxGeometry(rayData.GetWidth(),rayData.GetHeight(),rayData.GetDepth()),*mMaterial);
	//	break;
	// }
	case eColliderType::CONVEX:
	{
		const auto convex = collider->GetColliderAssetOfType<ColliderAssetConvex>();
		const auto convexData = convex->GetColliderMesh();
		// PxRigidActorExt::createExclusiveShape(*data,PxConvexMeshGeometry(Shipyard_PhysX::CookMesh(convexData)),*Shipyard_PhysXInstance.GetDefaultMaterial());
		break;
	}
	case eColliderType::PLANAR:
	{
		// TODO: reenable planar mesh gen
		// const auto convex = collider->GetColliderAssetOfType<ColliderAssetPlanar>();
		// const auto convexData = convex->GetColliderMesh();
		// const Vector3f scaleV = transform.GetScale();
		// PxVec3 scale = { scaleV.x,scaleV.y,scaleV.z };
		// PxRigidActorExt::createExclusiveShape(*data,
		//	PxTriangleMeshGeometry(Shipyard_PhysX::CookMesh<physx::PxTriangleMesh>(convexData),scale),
		//	*Shipyard_PhysXInstance.GetDefaultMaterial());
		// break;
	}
	default:
		auto geometry = PxBoxGeometry(1,1,1);
		shape->setGeometry(geometry);
		break;
	}

	auto pxTransform = [](const Transform& aTransform) {
		const Vector3f pos = aTransform.GetPosition(WORLD);
		const Quaternionf quat = aTransform.GetQuatF();
		return PxTransform(PxVec3(pos.x,pos.y,pos.z),
			PxQuat(quat.x,quat.y,quat.z,quat.w));
		};
	
	const auto quat = Quaternion(transform.unmodified_WorldMatrix().rotationMatrix()); // shits fucked here around
	//const PxMat44T<float> pxMatrix = PxMat44T<float>(&mat3x3.Transpose(),transform.unmodified_WorldMatrix().position());
	data->setGlobalPose(PxTransform(transform.unmodified_WorldMatrix().position(),quat));
}

void cPhysXStaticBody::Update() {
	UpdateFromCollider();
}

void cPhysXStaticBody::Destroy() {
	if(data) {
		data->release();
	}
}

void cPhysXStaticBody::OnSiblingChanged(const std::type_info* SourceClass) {
	OPTICK_EVENT();
	if(SourceClass == &typeid(Collider)) {
		UpdateFromCollider();
	}
}
