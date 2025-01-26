#include "PersistentSystems.pch.h"

#include "../Raycast.h"

#include <numbers>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include "Engine/PersistentSystems/Scene.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxActor.h"
#include "PxRigidActor.h"

namespace Physics {
	RaycastHit::RaycastHit() = default;
	RaycastHit::operator bool() {
		return (objectHit.IsValid());
	}

	RaycastHit::operator bool() const {
		return (objectHit.IsValid());
	}

	bool physxRaycast(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,GameObject filter,Layer layerMask,
		float    maxDistance) {
		filter;

		const  Vector3f directionNormalized = direction.GetNormalized();
		const physx::PxVec3 ori(origin.x,origin.y,origin.z);
		const physx::PxVec3 dir(directionNormalized.x,directionNormalized.y,directionNormalized.z);

		physx::PxRaycastBuffer pxhitinfo;
		auto filterData = physx::PxQueryFilterData();
		filterData.data.word0 = static_cast<unsigned int>(layerMask);

		Shipyard_PhysXInstance.GetScene()->raycast(ori,dir,maxDistance,pxhitinfo,physx::PxHitFlag::eDEFAULT,filterData);

		if(pxhitinfo.hasAnyHits()) {
			outHitInfo.point = Vector3f(pxhitinfo.block.position.x,pxhitinfo.block.position.y,pxhitinfo.block.position.z);
  			outHitInfo.normal = Vector3f(pxhitinfo.block.normal.x,pxhitinfo.block.normal.y,pxhitinfo.block.normal.z);
			outHitInfo.distance = pxhitinfo.block.distance;

			const SY::UUID* uuid = static_cast<SY::UUID*>(pxhitinfo.block.actor->userData);


			if(GameObject ptr = Scene::activeManager().GetGameObject(*uuid)) {
				outHitInfo.objectHit = ptr;
			} else {
				outHitInfo.objectHit = GameObject();
			}
			return true;
		}

		return false;
	}

	bool RaycastAll(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,GameObject filter,Layer layerMask,
		float    maxDistance) //
	{
		OPTICK_EVENT();
		layerMask;
		maxDistance;
		// REFACTOR, here usually we would use our nice world cached grid for collission
		// but i am bad at programming and even worse at getting up in the morning so it is not done
		//  yet 

		if(physxRaycast(origin,direction,outHitInfo,filter,layerMask,maxDistance)) {
			return true;
		}
	}

	bool Raycast(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,GameObject filter,Layer layerMask,
		float    maxDistance) //
	{
		OPTICK_EVENT();
		layerMask;
		maxDistance;
		// REFACTOR, here usually we would use our nice world cached grid for collission
		// but i am bad at programming and even worse at getting up in the morning so it is not done
		//  yet 

		if(physxRaycast(origin,direction,outHitInfo,filter,layerMask,maxDistance)) {
			return true;
		}

		float    distance = std::powf(maxDistance,2);
		Vector3f hitPoint;
		for(auto& i : Scene::activeManager().GetAllComponents<Collider>()) // Observe crabLiftingBar.jpeg
		{
			Layer a = layerMask;
			Layer b = i.gameObject().GetLayer();

			const bool f = (static_cast<int>(a) & static_cast<int>(b));
			if(!f // plz fix u stupid bastard
				|| !i.IsActive() || i.GetOwner() == filter.GetID()) {
				continue;
			}

			const auto& collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->ScaledAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			if(IntersectionAABBRay(collider,ray,hitPoint)) {
				const float newDistance = (hitPoint - origin).LengthSqr();
				if(newDistance < distance) {
					distance = newDistance;
					outHitInfo.point = hitPoint;
					outHitInfo.objectHit = i.gameObject();
				}
			}
		}
		return distance < maxDistance;
	}

	bool Raycast(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,Layer layerMask,float maxDistance) //
	{
		OPTICK_EVENT();
		layerMask;
		maxDistance;
		// REFACTOR, here usually we would use our nice world cached grid for collission
		// but i am bad at programming and even worse at getting up in the morning so it is not done
		//  yet

		// Observe crabLiftingBar.jpeg

		if(physxRaycast(origin,direction,outHitInfo,GameObject(),layerMask,maxDistance)) {
			return true;
		}

		float    distance = std::powf(maxDistance,2);
		Vector3f hitPoint;
		for(auto& i : Scene::activeManager().GetAllComponents<Collider>()) {
			auto& collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->ScaledAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			if(IntersectionAABBRay(collider,ray,hitPoint)) {
				const float newDistance = (hitPoint - origin).LengthSqr();
				if(newDistance < distance) {
					distance = newDistance;
					outHitInfo.point = hitPoint;
					outHitInfo.objectHit = i.gameObject();
				}
			}
		}
		return distance < maxDistance;
	}

	bool Raycast(Vector3f origin,Vector3f direction,Layer layerMask,float maxDistance) //
	{
		OPTICK_EVENT();
		layerMask;
		maxDistance;
		// REFACTOR, here usually we would use our nice world cached grid for collission
		// but i am bad at programming and even worse at getting up in the morning so it is not done
		//  yet
		auto dumpValue = RaycastHit();
		if(physxRaycast(origin,direction,dumpValue,GameObject(),layerMask,maxDistance)) {
			return true;
		}

		const float    distance = std::powf(maxDistance,2);
		const Vector3f hitPoint;
		// Observe crabLiftingBar.jpeg
		for(auto& i : Scene::activeManager().GetAllComponents<Collider>()) {
			auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->ScaledAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			const float newDistance = (hitPoint - origin).LengthSqr();
			if(newDistance < distance) {
				return true;
			}
		}
		return false;
	}
} // namespace Physics
