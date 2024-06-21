#include "Engine/AssetManager/AssetManager.pch.h"
#include "../CombatComponent.h"
#include <Engine/AssetManager/ComponentSystem/Components/TaskSpecific/ProjectileComponent.h>

#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include "Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h"
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>

CombatComponent::CombatComponent(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
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
	spawnPoint = GetGameObject().GetComponent<Transform>().GetPosition();

}

void CombatComponent::Update()
{
	myAttackTimer += Timer::GetDeltaTime();

	if(myHealth <= 0)
	{
		//'John is kill'
		//'No' 
		myDeathTimer += Timer::GetDeltaTime();
		if(myDeathTimer > respawnTime)
		{
			myDeathTimer = 0.f;
			Respawn();
		}
	}
}

void CombatComponent::Render()
{
}

void CombatComponent::Respawn()
{
	auto& physicsComponent = GetGameObject().GetComponent<cPhysics_Kinematic>();
	physicsComponent.ph_Angular_velocity = {0,0,0};

	myHealth = 100;
	GetGameObject().GetComponent<Transform>().SetPosition(
		RandomEngine::RandomInRange<float>(-20,20),0,RandomEngine::RandomInRange<float>(-20,20)
	);
}

void CombatComponent::FireProjectile()
{
	if(myAttackTimer > myAttackSpeed)
	{
		myAttackTimer = 0.f;
		//Fire projectile
		GameObject projectile = GameObject::Create();


		auto& transform = projectile.AddComponent <Transform>();
		auto& kinematic = projectile.AddComponent <cPhysics_Kinematic>();
		auto& mesh = projectile.AddComponent <cMeshRenderer>();
		auto& collider = projectile.AddComponent <cCollider>();
		auto& projComponent = projectile.AddComponent <ProjectileComponent>();
		projComponent.InitWithValues(20.f,GetGameObject());

		transform.SetPosition(GetGameObject().GetComponent<Transform>().GetPosition());
		transform.SetRotation(GetGameObject().GetComponent<Transform>().GetRotation());
		transform.Rotate(90,0,0);
		kinematic.ph_velocity = GetGameObject().GetComponent<Transform>().GetForward() * myProjectileSpeed;
		mesh.SetNewMesh("Models/Projectile.fbx");
		projectile.SetLayer(Layer::Projectile);
		collider;
	}
}
void CombatComponent::Healing()
{
	decimalHPGeneration += 2.f * Timer:: GetDeltaTime();

	//KEKW
	const int hpAdd = static_cast<int>(std::roundf(decimalHPGeneration));
	decimalHPGeneration -= hpAdd;
	myHealth += hpAdd;

	if(myHealth > 100)
	{
		myHealth = 100;
	}
}
