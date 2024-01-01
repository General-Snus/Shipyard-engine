#include "../ProjectileComponent.h"
#include "AssetManager.pch.h"

ProjectileComponent::ProjectileComponent(const SY::UUID anOwnerID) : Component(anOwnerID)
{
}

void ProjectileComponent::Init()
{

}

void ProjectileComponent::Update()
{
	lifetime -= Timer::GetInstance().GetDeltaTime();
	if(lifetime <= 0)
	{
		GameObjectManager::Get().DeleteGameObject(myOwnerID);
	}
}

void ProjectileComponent::InitWithValues(float aLifetime,GameObject aCreator)
{
	lifetime = aLifetime;
	Creator = aCreator;
}

void ProjectileComponent::CollidedWith(const SY::UUID aGameObjectID)
{
	if(aGameObjectID != Creator.GetID())
	{
		if(auto* arg = GameObjectManager::Get().TryGetComponent<CombatComponent>(aGameObjectID))
		{
			arg->myHealth -= 10;
		}
		// doing this because have no nice tag system :((( 
		if(auto* arg = GameObjectManager::Get().TryGetComponent<ProjectileComponent>(aGameObjectID))
		{
			return;
		}
		GameObjectManager::Get().DeleteGameObject(myOwnerID);
	}
}
