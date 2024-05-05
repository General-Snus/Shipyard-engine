#pragma once
#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <numbers>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>


namespace Physics {
	struct RaycastHit
	{
		GameObject objectHit;
		Vector3f point;
		Vector3f normal;
	};

	static bool Raycast(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,GameObject filter,Layer layerMask = Layer::AllLayers,float maxDistance = FLT_MAX) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet



		float distance = maxDistance;
		Vector3f hitPoint;
		for (auto& i : GameObjectManager::Get().GetAllComponents<cCollider>()) //Observe crabLiftingBar.jpeg
		{
			Layer a = layerMask;
			Layer b = i.GetGameObject().GetLayer();

			bool f = ((int)a & (int)b);
			if (!f			// plz fix u stupid bastard
				|| !i.IsActive()
				|| i.GetOwner() == filter.GetID())
			{
				continue;
			}

			const auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			if (IntersectionAABBRay(collider,ray,hitPoint))
			{
				const float newDistance = (hitPoint - origin).Length();
				if (newDistance < distance)
				{
					distance = newDistance;
					outHitInfo.point = hitPoint;
					outHitInfo.objectHit = i.GetGameObject();
				}
			}
		}
		return distance < maxDistance;
	}

	static bool Raycast(Vector3f origin,Vector3f direction,RaycastHit& outHitInfo,Layer layerMask = Layer::AllLayers,float maxDistance = FLT_MAX) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet

		//Observe crabLiftingBar.jpeg

		float distance = maxDistance;
		Vector3f hitPoint;
		for (auto& i : GameObjectManager::Get().GetAllComponents<cCollider>())
		{
			auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			if (IntersectionAABBRay(collider,ray,hitPoint))
			{
				const float newDistance = (hitPoint - origin).Length();
				if (newDistance < distance)
				{
					distance = newDistance;
					outHitInfo.point = hitPoint;
					outHitInfo.objectHit = i.GetGameObject();
				}
			}
		}
		return distance < maxDistance;
	}

	static bool Raycast(Vector3f origin,Vector3f direction,Layer layerMask = Layer::AllLayers,float maxDistance = FLT_MAX) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet


		float distance = 0;
		distance;
		//Observe crabLiftingBar.jpeg
		for (auto& i : GameObjectManager::Get().GetAllComponents<cCollider>())
		{
			auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin,direction);

			if (IntersectionAABBRay(collider,ray))
			{
				return true;
			}
		}
	}

}

