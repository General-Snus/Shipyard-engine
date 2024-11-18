#pragma once
#include "../../Component.h"

class CombatComponent : public Component
{
  public:
    MYLIB_REFLECTABLE();
    CombatComponent(const SY::UUID anOwnerID, GameObjectManager *aManager);
    ~CombatComponent() override = default;
    void Init() override;
    void Update() override;
    void Respawn();
    void FireProjectile();
    void Healing();

    bool InspectorView() override;

    // Lazy town over here
    int myHealth;
    int myDamage;
    float myProjectileSpeed;
    float myAttackSpeed;
    float myAttackRange;
    float myVisionRange;
    float myAttackCone; // Degrees
    float respawnTime;

  private:
    float decimalHPGeneration = 0;
    Vector3f spawnPoint;
    float myDeathTimer;
    float myAttackTimer = 0;
};

REFL_AUTO(type(CombatComponent), field(myHealth), field(myDamage), field(myProjectileSpeed), field(myAttackSpeed),
          field(myAttackRange), field(myVisionRange), field(myAttackCone), field(respawnTime))
