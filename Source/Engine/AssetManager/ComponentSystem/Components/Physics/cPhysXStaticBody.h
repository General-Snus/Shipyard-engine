#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>

class cPhysXStaticBody : public Component
{
  public:
    ReflectableTypeRegistration();
    defaultInspector();
    cPhysXStaticBody(const SY::UUID anOwnerId, GameObjectManager *aManager);

    void Init() override;
    void UpdateFromCollider();
    void Update() override; 
    void Destroy() override;
    void OnSiblingChanged(const std::type_info *SourceClass) override;

  private:
    physx::PxRigidStatic *data; 
    physx::PxShape* shape = {}; //TODO: FIX THIS
};

REFL_AUTO(type(cPhysXStaticBody))
