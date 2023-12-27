#include "AssetManager.pch.h"
#include "../CombatComponent.h"

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
	myAttackCone = 45.f;
}

void CombatComponent::Update()
{
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
