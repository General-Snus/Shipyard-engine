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
	physicsComponent->ph_velocity = 5.0f * GlobalFwd;
	physicsComponent->ph_Angular_velocity = {0, 150 * RandomEngine::RandomBinomial(), 0};
	//physicsComponent->ph_Angular_velocity = {150 * RandomEngine::RandomBinomial(), 150 * RandomEngine::RandomBinomial(), 0}; 3d!


	{//Special circumstance limit area
		const float mapsize = 50.0f;
		auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code
		Vector3f position = transform.GetPosition();//varför funkar inte mod som i matte :,( hur var det detta som tog längst tid av allt
		position = {
			(Mod<float>((position.x),mapsize)),
			//(Mod<float>((position.y),mapsize*.5)),
			position.y,
			(Mod<float>((position.z),mapsize))
		};
		transform.SetPosition(position);
		return true;
	}
}

void WanderController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
