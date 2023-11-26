#pragma once
#include <Tools/Utilities/Math.hpp>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include "../AIEventManager.h"
 
enum class eControllerType : int
{
	dummy,player,event,polling,count
};

class Controller
{
public:
	virtual bool Update(GameObject input) = 0;
	virtual void Recieve(const AIEvent& aEvent);

protected: 
	eControllerType controllerType;
}; 