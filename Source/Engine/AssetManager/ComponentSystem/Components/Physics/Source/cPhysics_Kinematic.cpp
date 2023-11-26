#include "AssetManager.pch.h"
#include "../cPhysics_Kinematic.h"

cPhysics_Kinematic::cPhysics_Kinematic(const SY::UUID anOwnerID) : Component(anOwnerID)
{
	ph_velocity = {0.0f, 0.0f, 0.0f};
	ph_acceleration = {0.0f, 0.0f, 0.0f};
	ph_Angular_velocity = {0.0f, 0.0f, 0.0f};
	ph_Angular_acceleration = {0.0f, 0.0f, 0.0f};
}

void cPhysics_Kinematic::Init()
{
	//Check for required components
	if(!TryGetComponent<Transform>())
	{
		this->GetGameObject().AddComponent<Transform>();
	}
}

void cPhysics_Kinematic::Update()
{
	float delta = Timer::GetInstance().GetDeltaTime();
	if(auto* transform = TryGetComponent<Transform>())
	{
		ph_velocity += ph_acceleration * delta;
		ph_Angular_velocity += ph_Angular_acceleration * delta;

		if(localVelocity)
		{
			transform->Rotate(ph_Angular_velocity * delta);
			const Vector4f globalVelocity = Vector4f(ph_velocity,0);
			const Vector4f localVel = globalVelocity * transform->GetTransform();
			transform->Move(Vector3f(localVel.x,localVel.y,localVel.z) * delta);
		}
		transform->Rotate(ph_Angular_velocity * delta);
		transform->Move(ph_velocity * delta);

		//Todo
	/*
	if bound convert velocity to angle and set rotation
	*/
	}
}

void cPhysics_Kinematic::Render()
{
	//Render gizmo for velocity and acceleration
}
