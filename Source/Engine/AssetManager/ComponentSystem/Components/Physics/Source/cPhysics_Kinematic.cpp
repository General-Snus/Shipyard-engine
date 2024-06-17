#include "AssetManager.pch.h"
#include "../cPhysics_Kinematic.h"


cPhysics_Kinematic::cPhysics_Kinematic(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{
	ph_velocity = { 0.0f, 0.0f, 0.0f };
	ph_acceleration = { 0.0f, 0.0f, 0.0f };
	ph_Angular_velocity = { 0.0f, 0.0f, 0.0f };
	ph_Angular_acceleration = { 0.0f, 0.0f, 0.0f };
	ph_maxSpeed = 100.0f;
	ph_maxAcceleration = 100.0f;
}

cPhysics_Kinematic::~cPhysics_Kinematic()
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();
}

void cPhysics_Kinematic::Init()
{
	//Check for required components
	if (!TryGetComponent<Transform>())
	{
		this->GetGameObject().AddComponent<Transform>();
		InitPrimitive();
	}
}

void cPhysics_Kinematic::InitPrimitive()
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	const Vector3f position = GetComponent<Transform>().GetPosition();
	//Velocity  
	DebugDrawer::Get().AddDebugLine(position,position + ph_velocity,Vector3f(1.0f,0.0f,0.0f),.01f);

	//Acceleration
	DebugDrawer::Get().AddDebugLine(position,position + ph_acceleration,Vector3f(0.0f,0.0f,1.0f),.01f);

}

void cPhysics_Kinematic::Update()
{
	OPTICK_EVENT();
	float delta = Timer:: GetDeltaTime();
	auto& transform = GetComponent<Transform>();
	ph_velocity += ph_acceleration * delta;
	ph_Angular_velocity += ph_Angular_acceleration * delta;

	if (ph_velocity.Length() > ph_maxSpeed)
	{
		ph_velocity.Normalize();
		ph_velocity *= ph_maxSpeed;
	}

	if (ph_acceleration.Length() > ph_maxAcceleration)
	{
		ph_acceleration.Normalize();
		ph_acceleration *= ph_maxAcceleration;
	}

	if (localVelocity)
	{
		transform.Rotate(ph_Angular_velocity * delta);
		const Vector4f globalVelocity = Vector4f(ph_velocity,0);
		const Vector4f localVel = globalVelocity * transform.GetTransform();
		transform.Move(Vector3f(localVel.x,localVel.y,localVel.z) * delta);
		return;
	}

	transform.Rotate(ph_Angular_velocity * delta);
	transform.Move(ph_velocity * delta);
}

void cPhysics_Kinematic::Render()
{
	//Render gizmo for velocity and acceleration
#ifdef _DEBUGDRAW 

	if (auto* transform = TryGetComponent<Transform>())
	{
		InitPrimitive();
	}

#endif // _DEBUGDRAW 
}
