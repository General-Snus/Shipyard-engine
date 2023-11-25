#pragma once
#include <Tools/Utilities/Math.hpp>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include "../AIEventManager.h"

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
	virtual SteeringOutput Update(GameObject input) = 0;
	virtual void Recieve(const AIEvent& aEvent);

protected:
	Vector3f targetPosition; 
	eControllerType controllerType;
}; 