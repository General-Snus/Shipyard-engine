#include "AssetManager.pch.h"
//MOTHBALLED
#ifdef  MOTHBALLED 
#include "Actor.h"  
SteeringOutput Actor::Update(const SteeringInput& input)
{
	input;
	if (controller)
	{
		//return controller->Update(input); //Velocity
	}
	return SteeringOutput();
}

void Actor::SetController(Controller* aController)
{
	controller = aController;
}
#endif