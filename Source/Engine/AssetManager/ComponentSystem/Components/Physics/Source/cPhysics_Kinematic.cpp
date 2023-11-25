#include "AssetManager.pch.h"
#include "../cPhysics_Kinematic.h"

void cPhysics_Kinematic::Init()
{
	this->TryGetAddComponent<Transform>();
}

void cPhysics_Kinematic::Update()
{
	float delta = Timer::GetInstance().GetDeltaTime();
	if(auto* transform = TryGetComponent<Transform>())
	{
		ph_velocity += ph_acceleration * delta;
		transform->Move(ph_velocity * delta);
	}
}

void cPhysics_Kinematic::Render()
{
	//Render gizmo for velocity and acceleration
}

void cPhysics_Kinematic::SetVelocity(const Vector3f& aVelocity)
{
	ph_velocity = aVelocity;
}

void cPhysics_Kinematic::SetAcceleration(const Vector3f& anAcceleration)
{
	ph_acceleration = anAcceleration;
}

void cPhysics_Kinematic::AddVelocity(const Vector3f& aVelocity)
{
	ph_velocity += aVelocity;
}

void cPhysics_Kinematic::AddAcceleration(const Vector3f& anAcceleration)
{
	ph_acceleration += anAcceleration;
}
