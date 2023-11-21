#include "Actor.h"  
Vector3f Actor::Update(float aTimeDelta)
{
	if(controller)
	{ 
		return controller->Update(position).GetNormalized() * aTimeDelta; //Velocity
	}
	return Vector3f(0,0,0);
}
 

void Actor::SetController(Controller* aController)
{
	controller = aController;
}
