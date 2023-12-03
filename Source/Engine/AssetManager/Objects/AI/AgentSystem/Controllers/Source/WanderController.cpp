#include "AssetManager.pch.h"
#include "../WanderController.h"
#include <Tools/Utilities/Math.hpp> 

WanderController::WanderController(GameObject componentCheck)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = true;
	}
}

bool WanderController::Update(GameObject input)
{
	auto* physicsComponent = input.TryGetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>();

	Vector3f position = transform.GetPosition();

	physicsComponent->ph_velocity = 5.0f * GlobalFwd;
	//physicsComponent->ph_Angular_velocity = {0, 30 * RandomEngine::RandomBinomial(), 0};
	physicsComponent->ph_Angular_velocity = {0,std::powf(RandomEngine::RandomBinomial(),5) * 3600, 0};
	//physicsComponent->ph_Angular_velocity = {150 * RandomEngine::RandomBinomial(), 150 * RandomEngine::RandomBinomial(), 0}; 3d!


	transform.SetPosition(SteeringBehaviour::SetPositionInBounds(position,50.0f));
	return true;
}

void WanderController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
