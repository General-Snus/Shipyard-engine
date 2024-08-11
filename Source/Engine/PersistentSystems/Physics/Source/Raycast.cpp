#include "Engine\PersistentSystems\PersistentSystems.pch.h"

#include "../Raycast.h"


#include <numbers>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

#include "Engine/PersistentSystems/Scene.h"
#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

namespace Physics {

	bool Raycast(Vector3f origin, Vector3f direction, RaycastHit& outHitInfo, GameObject filter, Layer layerMask, float maxDistance) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet



		float distance = std::powf(maxDistance, 2);
		Vector3f hitPoint;
		for (auto& i : Scene::ActiveManager().GetAllComponents<cCollider>()) //Observe crabLiftingBar.jpeg
		{
			Layer a = layerMask;
			Layer b = i.GetGameObject().GetLayer();

			bool f = (static_cast<int>(a) & static_cast<int>(b));
			if (!f			// plz fix u stupid bastard
				|| !i.IsActive()
				|| i.GetOwner() == filter.GetID())
			{
				continue;
			}

			const auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin, direction);

			if (IntersectionAABBRay(collider, ray, hitPoint))
			{
				const float newDistance = (hitPoint - origin).LengthSqr();
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

	bool Raycast(Vector3f origin, Vector3f direction, RaycastHit& outHitInfo, Layer layerMask, float maxDistance) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet

		//Observe crabLiftingBar.jpeg

		float distance = std::powf(maxDistance, 2);
		Vector3f hitPoint;
		for (auto& i : Scene::ActiveManager().GetAllComponents<cCollider>())
		{
			auto& collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin, direction);

			if (IntersectionAABBRay(collider, ray, hitPoint))
			{
				const float newDistance = (hitPoint - origin).LengthSqr();
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

	bool Raycast(Vector3f origin, Vector3f direction, Layer layerMask, float maxDistance) //
	{
		OPTICK_EVENT();
		layerMask; maxDistance;
		//REFACTOR, here usually we would use our nice world cached grid for collission
		//but i am bad at programming and even worse at getting up in the morning so it is not done
		// yet


		float distance = std::powf(maxDistance, 2);
		Vector3f hitPoint;
		//Observe crabLiftingBar.jpeg
		for (auto& i : Scene::ActiveManager().GetAllComponents<cCollider>())
		{
			auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			Ray ray;
			ray.InitWithOriginAndDirection(origin, direction);

			const float newDistance = (hitPoint - origin).LengthSqr();
			if (newDistance < distance)
			{
				return true;
			}
		}
		return false;
	}

}

