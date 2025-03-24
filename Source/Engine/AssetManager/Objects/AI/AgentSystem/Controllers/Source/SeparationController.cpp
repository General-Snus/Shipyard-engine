#include "AssetManager.pch.h"
#include "../SeparationController.h"
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>

#include "Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/SteeringBehaviour.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h"

SeparationController::SeparationController(MultipleTargets_PollingStation* station,GameObject componentCheck) : PollingController(station)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 5.0f;
	}
}

bool SeparationController::Update(GameObject input)
{
	// You can use Get directly if you are sure it will exist, its faster and force safe code
	auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>();

	const auto fwd = transform.GetForward();
	const Vector3f position = transform.GetPosition();
	const auto arg = reinterpret_cast<MultipleTargets_PollingStation*>(pollingStation)->GetTargetPosition();
	 
	SteeringBehaviour::DampenVelocity(&physicsComponent);
	SteeringBehaviour::Separation(arg,&physicsComponent,position,input.GetID());
	SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,fwd);
	transform.SetPosition(SteeringBehaviour::SetPositionInBounds(position,50.0f));

	return true;
}


void SeparationController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
