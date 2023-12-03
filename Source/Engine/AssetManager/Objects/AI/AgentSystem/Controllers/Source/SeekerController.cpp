#include "AssetManager.pch.h"
#include "../SeekerController.h"

SeekerController::SeekerController(MultipleTargets_PollingStation* aPollingStation,MultipleTargets_PollingStation* formationStation,GameObject componentCheck) : PollingController(aPollingStation),FormationStation(formationStation)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 4.5f;
	}
}

bool SeekerController::Update(GameObject input)
{
	auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code

	Vector3f position = transform.GetPosition();
	Vector3f fwd = transform.GetForward();
	Vector3f targetPos = reinterpret_cast<MultipleTargets_PollingStation*>(pollingStation)->GetClosestTargetPosition(position);
	Vector3f direction = (targetPos - position).GetNormalized();

	SteeringBehaviour::DampenVelocity(&physicsComponent);
	SteeringBehaviour::LookAt(&physicsComponent,direction,fwd,5.0f);
	physicsComponent.ph_acceleration += 5.0f * direction;

	auto arg = reinterpret_cast<MultipleTargets_PollingStation*>(FormationStation)->GetTargetPosition();
	SteeringBehaviour::SeparationSettings settings;
	settings.decayCoefficient = 10.0f;
	settings.threshold = 2.0f;
	SteeringBehaviour::Separation(arg,&physicsComponent,position,input.GetID(),settings);

	SteeringBehaviour::Cohesion(&physicsComponent,position,reinterpret_cast<MultipleTargets_PollingStation*>(FormationStation),5.f,2.0f);
	//physicsComponent.ph_velocity.Normalize();
	//physicsComponent.ph_velocity *= 4.5f;

	transform.SetPosition(SteeringBehaviour::SetPositionInBounds(position,50.0f));

	return true;
}

void SeekerController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
