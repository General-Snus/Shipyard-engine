#pragma once
#include "../../LinearAlgebra/Vector3.hpp"
#include "AIEventManager.h"

enum class eControllerType : int
{
	dummy,player,event,polling,count
};

class Controller
{
public:
	virtual Vector3f Update(const Vector3f aPosition) = 0;
	virtual void Recieve(const AIEvent& aEvent);

protected:
	Vector3f targetPosition; 
	eControllerType controllerType;
}; 