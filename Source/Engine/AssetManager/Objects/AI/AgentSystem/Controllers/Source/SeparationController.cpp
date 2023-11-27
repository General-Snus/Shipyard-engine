#include "AssetManager.pch.h"
#include "../SeparationController.h"

SeparationController::SeparationController(MultipleTargets_PollingStation* station,GameObject componentCheck) : PollingController(station)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = true;
		phy.ph_maxSpeed = 5.0f;
	}

}

bool SeparationController::Update(GameObject input)
{
	auto* physicsComponent = input.TryGetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code

	//Dampening
 	physicsComponent->ph_acceleration = -physicsComponent->ph_velocity.GetNormalized() *.1f;

	for(auto const& i : reinterpret_cast<MultipleTargets_PollingStation*>(pollingStation)->GetTargetPosition())
	{
		if(i.sourceObject.GetID() == input.GetID())
		{
			continue;
		}
		Vector3f direction = (i.positionData - transform.GetPosition());
		float distance = direction.Length();
		if(distance < threshold)
		{
			float strength = std::min(
				decayCoefficient / (distance * distance),
				maxAcceleration);

			direction.Normalize();
			physicsComponent->ph_acceleration += strength * -direction;
		}
	}
	

	{//Special circumstance limit area
		const float mapsize = 50.0f;
		Vector3f position = transform.GetPosition();//varför funkar inte mod som i matte :,( hur var det detta som tog längst tid av allt
		position = {
			(Mod<float>((position.x),mapsize)),
			position.y,
			(Mod<float>((position.z),mapsize))
		};
		transform.SetPosition(position);
		return true;
	}
}

void SeparationController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
