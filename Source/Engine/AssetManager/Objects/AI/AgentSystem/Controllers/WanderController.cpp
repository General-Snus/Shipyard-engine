#include "AssetManager.pch.h"
#include "WanderController.h"
#include <Tools/Utilities/Math.hpp> 

WanderController::WanderController(const PollingStation& aPollingStation,GameObject componentCheck) : PollingController(aPollingStation) 
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
	physicsComponent->ph_velocity = {0,0,1};
	physicsComponent->ph_Angular_velocity = {0, 15*RandomInRange(-0.0f,1.0f), 0};
	return true;
}

void WanderController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
