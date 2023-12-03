#include "AssetManager.pch.h"
#include "../SeekerController.h"

AIController::AIController(MultipleTargets_PollingStation* aPollingStation,MultipleTargets_PollingStation* formationStation,GameObject componentCheck) : PollingController(aPollingStation),FormationStation(formationStation)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 4.5f;
		phy.ph_maxAcceleration = 5.0f;
	}
	
}

bool AIController::Update(GameObject input)
{
	auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code
	SteeringBehaviour::SeparationSettings settings;
	float influenceRadius = 5.0f;
	Vector3f position = transform.GetPosition();
	Vector3f fwd = transform.GetForward();

	//SteeringBehaviour::DampenVelocity(&physicsComponent); // Dampen makes smoother movement but dampens resposiveness to high frequency response
	SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,fwd,5.0f); 

	SteeringBehaviour::Wander(&physicsComponent,fwd,5.0f);

	//Avg velocity within a circle
	Vector3f avgVelocity = reinterpret_cast<MultipleTargets_PollingStation*>(FormationStation)->GetAverageVelocityWithinCircle(position,influenceRadius);
	SteeringBehaviour::VelocityMatching(&physicsComponent,avgVelocity,1.0f);
	 

	auto arg = reinterpret_cast<MultipleTargets_PollingStation*>(FormationStation)->GetTargetPosition();
	settings.decayCoefficient = 10.0f;
	settings.threshold = 2.0f;
	SteeringBehaviour::Separation(arg,&physicsComponent,position,input.GetID(),settings);


	//Collider separation for seperating the groups to clearerer see behaviour
	auto colliders = reinterpret_cast<MultipleTargets_PollingStation*>(pollingStation)->GetTargetPosition(); 
	settings.decayCoefficient = 20.0f;
	settings.threshold = 5.0f;
	SteeringBehaviour::Separation(colliders,&physicsComponent,position,input.GetID(),settings);


	SteeringBehaviour::Cohesion(&physicsComponent,position,reinterpret_cast<MultipleTargets_PollingStation*>(FormationStation),influenceRadius,2.0f);
	//physicsComponent.ph_velocity.Normalize();
	//physicsComponent.ph_velocity *= 4.5f;

	transform.SetPosition(SteeringBehaviour::SetPositionInBounds(position,50.0f));

	return true;
}

void AIController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
