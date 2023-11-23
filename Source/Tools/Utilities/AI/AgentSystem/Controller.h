#pragma once
#include "../../LinearAlgebra/Vector3.hpp"
#include "AIEventManager.h"

struct SteeringOutput
{
	Vector3f movement;
	Vector3f rotation;
};

struct SteeringInput
{
	Vector3f position;
	Vector3f rotation;
};
enum class eControllerType : int
{
	dummy,player,event,polling,count
};

class Controller
{
public:
	virtual SteeringOutput Update(const SteeringInput& input) = 0;
	virtual void Recieve(const AIEvent& aEvent);

protected:
	Vector3f targetPosition; 
	eControllerType controllerType;
}; 