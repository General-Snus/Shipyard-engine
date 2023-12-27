#pragma once 
#include "../../Component.h"

class CombatComponent : public Component
{
public:
	CombatComponent(const SY::UUID anOwnerID);
	~CombatComponent() = default;
	void Init() override;
	void Update() override;
	void Render() override;

	//Lazy town over here
	int myHealth;
	int myDamage;
	float myProjectileSpeed;
	float myAttackSpeed;
	float myAttackRange;
	float myAttackCone; // Degrees
};
