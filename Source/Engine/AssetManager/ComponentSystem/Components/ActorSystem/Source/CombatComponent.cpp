#include "../CombatComponent.h"
#include "AssetManager.pch.h"

CombatComponent::CombatComponent(const SY::UUID anOwnerID) : Component(anOwnerID)
{
}

void CombatComponent::Init()
{
	myHealth = 100;
	myDamage = 10;
	myProjectileSpeed = 10.f;
	myAttackSpeed = 1.f;
	myAttackRange = 10.f;
	myAttackCone = 5.f;
	myVisionRange = 20.f;
}

void CombatComponent::Update()
{
	myAttackTimer += Timer::GetInstance().GetDeltaTime();

	if(myHealth <= 0)
	{
		//'John is kill'
		//'No'
		std::cout << "kia" << std::endl;
	}
}

void CombatComponent::Render()
{
}

void CombatComponent::FireProjectile()
{
	if(myAttackTimer > myAttackSpeed)
	{
		myAttackTimer = 0.f;
		//Fire projectile
		GameObject projectile = GameObjectManager::Get().CreateGameObject();


		auto& transform = projectile.AddComponent <Transform>();
		auto& kinematic = projectile.AddComponent <cPhysics_Kinematic>();
		auto& mesh = projectile.AddComponent <cMeshRenderer>();
		auto& collider = projectile.AddComponent <cCollider>();
		collider;

		transform.SetPosition(GetGameObject().GetComponent<Transform>().GetPosition());
		transform.SetRotation(GetGameObject().GetComponent<Transform>().GetRotation());
		transform.Rotate(90,0,0);
		kinematic.ph_velocity = GetGameObject().GetComponent<Transform>().GetForward() * myProjectileSpeed;
		mesh.SetNewMesh("Models/Projectile.fbx");
	}
}
