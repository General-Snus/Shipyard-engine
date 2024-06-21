#include "Engine/AssetManager/AssetManager.pch.h"
#include "Engine/AssetManager/ComponentSystem/Components/TaskSpecific/ProjectileComponent.h"

#include "Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h"

ProjectileComponent::ProjectileComponent(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{
}

void ProjectileComponent::Init()
{

}

void ProjectileComponent::Update()
{
	lifetime -= Timer::GetDeltaTime();
	if (lifetime <= 0)
	{
		Scene::ActiveManager().DeleteGameObject(myOwnerID);
	}
}

void ProjectileComponent::InitWithValues(float aLifetime,GameObject aCreator)
{
	lifetime = aLifetime;
	Creator = aCreator;
}

void ProjectileComponent::CollidedWith(const SY::UUID aGameObjectID)
{
	if (aGameObjectID != Creator.GetID())
	{
		if (auto* arg = Scene::ActiveManager().TryGetComponent<CombatComponent>(aGameObjectID))
		{
			arg->myHealth -= 10;
		}
		// doing this because have no nice tag system :((( 
		if (auto* arg = Scene::ActiveManager().TryGetComponent<ProjectileComponent>(aGameObjectID))
		{
			return;
		}
		Scene::ActiveManager().DeleteGameObject(myOwnerID);
	}
}
