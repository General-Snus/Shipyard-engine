#include "AssetManager.pch.h"
#include "../cPhysics_Kinematic.h"
#include <Engine/GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_DrawDebugPrimitive.h>

cPhysics_Kinematic::cPhysics_Kinematic(const SY::UUID anOwnerID) : Component(anOwnerID)
{
	ph_velocity = {0.0f, 0.0f, 0.0f};
	ph_acceleration = {0.0f, 0.0f, 0.0f};
	ph_Angular_velocity = {0.0f, 0.0f, 0.0f};
	ph_Angular_acceleration = {0.0f, 0.0f, 0.0f};
	ph_maxSpeed = 100.0f;
}

void cPhysics_Kinematic::Init()
{
	//Check for required components
	if(!TryGetComponent<Transform>())
	{
		this->GetGameObject().AddComponent<Transform>();
		InitPrimitive();
	} 
}
void cPhysics_Kinematic::InitPrimitive()
{

	for(DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	Matrix rf = GetComponent<Transform>().GetTransform();
	if(!localVelocity)
	{
		rf = Matrix::CreateTranslationMatrix(GetComponent<Transform>().GetPosition());
	}

	myHandles.clear();
	//Velocity  
	DebugDrawer::PrimitiveHandle handle = DebugDrawer::Get().AddDebugLine({0,0,0},ph_velocity,Vector3f(1.0f,0.0f,0.0f));
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle,rf);
	myHandles.push_back(handle);
	//Acceleration
	handle = DebugDrawer::Get().AddDebugLine({0,0,0},ph_acceleration,Vector3f(0.0f,0.0f,1.0f));
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle,rf);
	myHandles.push_back(handle);
}

void cPhysics_Kinematic::Update()
{
	float delta = Timer::GetInstance().GetDeltaTime();
	if(auto* transform = TryGetComponent<Transform>())
	{
		ph_velocity += ph_acceleration * delta;
		ph_Angular_velocity += ph_Angular_acceleration * delta;
		
		if(ph_velocity.Length() > ph_maxSpeed)
		{
			ph_velocity.Normalize();
			ph_velocity *= ph_maxSpeed; 
		}

		if(localVelocity)
		{
			transform->Rotate(ph_Angular_velocity * delta);
			const Vector4f globalVelocity = Vector4f(ph_velocity,0);
			const Vector4f localVel = globalVelocity * transform->GetTransform();
			transform->Move(Vector3f(localVel.x,localVel.y,localVel.z) * delta);
			return;
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
#ifdef _DEBUGDRAW 

	if(auto* transform = TryGetComponent<Transform>())
	{
		 InitPrimitive();
	}
	 
#endif // _DEBUGDRAW 
}
