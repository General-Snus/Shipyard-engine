#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>

class cPhysics_Kinematic : public Component
{
public:
	cPhysics_Kinematic(const SY::UUID anOwnerID);
	~cPhysics_Kinematic() override;
	void Init() override;
	void UpdatePrimitive();
	void Update() override;
	void Render() override;

	Vector3f ph_velocity;
	Vector3f ph_acceleration;
	Vector3f ph_Angular_acceleration;
	Vector3f ph_Angular_velocity;
	bool bindAngleToVelocity;
	bool localVelocity;
	float ph_maxSpeed;
	float ph_maxAcceleration;
private:
	void InitPrimitive();
	std::vector<DebugDrawer::PrimitiveHandle> myHandles;
};

REFL_AUTO(type(cPhysics_Kinematic))