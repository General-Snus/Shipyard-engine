#include "AssetManager.pch.h"

#include "Engine/AssetManager/ComponentSystem/Components/TaskSpecific/ProjectileComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h"

ProjectileComponent::ProjectileComponent(const SY::UUID anOwnerId, GameObjectManager* aManager)
	: Component(anOwnerId, aManager)
{
}

void ProjectileComponent::Init()
{
}

void ProjectileComponent::Update()
{
	lifetime -= TimerInstance.getDeltaTime();
	if (lifetime <= 0)
	{
		Scene::activeManager().DeleteGameObject(myOwnerID);
	}
}

void ProjectileComponent::InitWithValues(float aLifetime, GameObject aCreator)
{
	lifetime = aLifetime;
	Creator = aCreator;
}

void ProjectileComponent::OnColliderEnter(const SY::UUID aGameObjectID)
{
	if (aGameObjectID != Creator.GetID())
	{
		if (auto* arg = Scene::activeManager().TryGetComponent<CombatComponent>(aGameObjectID))
		{
			arg->myHealth -= 10;
		}
		// doing this because have no nice tag system :(((
		if (auto* arg = Scene::activeManager().TryGetComponent<ProjectileComponent>(aGameObjectID))
		{
			return;
		}
		Scene::activeManager().DeleteGameObject(myOwnerID);
	}
}
