#include "PersistentSystems.pch.h"

#include "../OnCollision.h"

#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>

void CollisionChecks::RemoveCollisions(const SY::UUID &anOwnerID)

{
    OPTICK_EVENT();
    activeCollisions.erase(anOwnerID);
    for (auto &collisions : activeCollisions)
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
    int index = 1;
    auto listOfColliders = Scene::ActiveManager().GetAllComponents<Collider>();

    if (listOfColliders.size() <= 1)
    {
        return;
    }

    for (const auto &collider : listOfColliders)
    {
        const auto firstOwner = collider.GetOwner();
        if (!collider.IsActive() || !collider.GetGameObject().GetActive())
        {
            index++;
            continue;
        }

        const AABB3D<float> &firstCollider = collider.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
        for (size_t i = index; i < listOfColliders.size(); i++)
        {
            const auto secondOwner = listOfColliders[i].GetOwner();
            if (!listOfColliders[i].IsActive() || !listOfColliders[i].GetGameObject().GetActive())
            {
                continue;
            }

            const auto &secondCollider = listOfColliders[i].GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

            if (IntersectionAABB<float>(firstCollider, secondCollider))
            {
                if (activeCollisions[firstOwner][secondOwner] == true)
                {
                    // OnCollisionAlways()
                }
                else
                {
                    Scene::ActiveManager().OnColliderEnter(firstOwner, secondOwner);
                }

                activeCollisions[firstOwner][secondOwner] = true;
            }
            else
            {
                if (activeCollisions[firstOwner][secondOwner] == true)
                {
                    Scene::ActiveManager().OnColliderExit(collider.GetOwner(), secondOwner);
                    activeCollisions[collider.GetOwner()][secondOwner] = false;
                }
            }
        }
        index++;
    }
}
