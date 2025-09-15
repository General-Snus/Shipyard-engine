#pragma once
#include "../AIEventManager.h"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/Reflection/ReflectionTemplate.h>
#include <Tools/Utilities/Math.hpp>
#include "Engine\AssetManager\Interfaces.h"

enum class eControllerType : int
{
	dummy,
	player,
	event,
	polling,
	count
};

class Controller : public Reflectable<Controller>, public AvailableInInspector
{
public:
	reflectable(Controller);
	virtual bool Update(GameObject input) = 0;
	virtual void Recieve(const AIEvent& aEvent);
	virtual bool ComponentRequirement(GameObject input);

protected:
	eControllerType controllerType;
};

REFL_AUTO(type(Controller))
