#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>

class cPhysics_Kinematic : public Component
{
  public:
    ReflectableTypeRegistration();
    cPhysics_Kinematic(const SY::UUID anOwnerId, GameObjectManager *aManager);
    ~cPhysics_Kinematic() override;
    void Init() override;
    void UpdatePrimitive();
    void Update() override; 
    bool InspectorView() override;

    Vector3f ph_velocity;
    Vector3f ph_acceleration;
    Vector3f ph_Angular_acceleration;
    Vector3f ph_Angular_velocity;
    bool bindAngleToVelocity = false;
    bool localVelocity = false;
    float ph_maxSpeed = 100.0f;
    float ph_maxAcceleration = 100.0f;
    float ph_maxAngularAcceleration = 25.0f;

  private:
    void InitPrimitive();
    std::vector<DebugDrawer::PrimitiveHandle> myHandles;
};

REFL_AUTO(type(cPhysics_Kinematic), field(ph_velocity), field(ph_acceleration), field(ph_Angular_acceleration),
          field(ph_Angular_velocity), field(bindAngleToVelocity), field(localVelocity), field(ph_maxSpeed),
          field(ph_maxAcceleration), field(ph_maxAngularAcceleration))
