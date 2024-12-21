#include "PersistentSystems.pch.h"

#include "../OnCollision.h"

#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>

void CollisionChecks::RemoveCollisions(const SY::UUID& anOwnerID)

{
	OPTICK_EVENT();
	activeCollisions.erase(anOwnerID);
	for (auto& collisions : activeCollisions)
	{
		collisions.second.erase(anOwnerID);
	}
}

void CollisionChecks::Update(float delta)
{
	delta;
	CheckColliders();
}

void CollisionChecks::CheckColliders()
{
	OPTICK_EVENT();
	int        index = 1;
	const auto listOfColliders = Scene::activeManager().GetAllComponents<Collider>();

	if (listOfColliders.size() <= 1)
	{
		return;
	}

	for (const auto& collider : listOfColliders)
	{
		const auto firstOwner = collider.GetOwner();
		if (!collider.IsActive() || !collider.gameObject().GetActive())
		{
			index++;
			continue;
		}

		const AABB3D<float>& firstCollider = collider.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
		for (size_t i = index; i < listOfColliders.size(); i++)
		{
			const auto secondOwner = listOfColliders[i].GetOwner();
			if (!listOfColliders[i].IsActive() || !listOfColliders[i].gameObject().GetActive())
			{
				continue;
			}

			const auto& secondCollider = listOfColliders[i].GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

			if (IntersectionAABB<float>(firstCollider, secondCollider))
			{
				if (activeCollisions[firstOwner][secondOwner] == true)
				{
					// OnCollisionAlways()
				}
				else
				{
					Scene::activeManager().OnColliderEnter(firstOwner, secondOwner);
				}

				activeCollisions[firstOwner][secondOwner] = true;
			}
			else
			{
				if (activeCollisions[firstOwner][secondOwner] == true)
				{
					Scene::activeManager().OnColliderExit(collider.GetOwner(), secondOwner);
					activeCollisions[collider.GetOwner()][secondOwner] = false;
				}
			}
		}
		index++;
	}
}
