#include "AssetManager.pch.h"
#include "../CombatComponent.h"
#include <Engine/AssetManager/ComponentSystem/Components/TaskSpecific/ProjectileComponent.h>

#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include "Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h"

CombatComponent::CombatComponent(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
}

void CombatComponent::Init()
{
	myHealth = 100;
	myDamage = 10;
	myProjectileSpeed = 10.f;
	myAttackSpeed = 1.f;
	myAttackRange = 10.f;
	myAttackCone = 10.f;
	myVisionRange = 20.f;
	respawnTime = 10.f;
	myDeathTimer = 0.f;
	spawnPoint = gameObject().GetComponent<Transform>().GetPosition();
}

void CombatComponent::Update()
{
	myAttackTimer += TimerInstance.getDeltaTime();

	if (myHealth <= 0)
	{
		//'John is kill'
		//'No'
		myDeathTimer += TimerInstance.getDeltaTime();
		if (myDeathTimer > respawnTime)
		{
			myDeathTimer = 0.f;
			Respawn();
		}
	}
}

void CombatComponent::Respawn()
{
	auto& physicsComponent = gameObject().GetComponent<cPhysics_Kinematic>();
	physicsComponent.ph_Angular_velocity = {0, 0, 0};

	myHealth = 100;
	transform().SetPosition(Math::RandomEngine::randomInRange<float>(-20, 20), 0,
	                        Math::RandomEngine::randomInRange<float>(-20, 20));
}

void CombatComponent::FireProjectile()
{
	if (myAttackTimer > myAttackSpeed)
	{
		myAttackTimer = 0.f;
		// Fire projectile
		GameObject projectile = GameObject::Create("Projectile");
		auto&      kinematic = projectile.AddComponent<cPhysics_Kinematic>();
		auto&      mesh = projectile.AddComponent<MeshRenderer>();
		const auto&      collider = projectile.AddComponent<Collider>();
		auto&      projComponent = projectile.AddComponent<ProjectileComponent>();
		projComponent.InitWithValues(20.f, gameObject());

		auto& projectileT = projectile.transform();
		auto& actorT = transform();

		projectileT.SetPosition(actorT.GetPosition());
		projectileT.SetRotation(actorT.euler());
		projectileT.Rotate(90, 0, 0);
		kinematic.ph_velocity = actorT.GetForward() * myProjectileSpeed;

		if (const auto& actorRenderer = actorT.TryGetComponent<MeshRenderer>())
		{
			mesh.SetMaterial(actorRenderer->GetMaterial());
		}

		mesh.SetNewMesh("Models/Projectile.fbx");
		projectile.SetLayer(Layer::Projectile);
		collider;
	}
}


void CombatComponent::Healing()
{
	decimalHPGeneration += 2.f * TimerInstance.getDeltaTime();

	// KEKW
	const int hpAdd = static_cast<int>(std::roundf(decimalHPGeneration));
	decimalHPGeneration -= hpAdd;
	myHealth += hpAdd;

	if (myHealth > 100)
	{
		myHealth = 100;
	}
}

bool CombatComponent::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<CombatComponent>();
	return true;
}
