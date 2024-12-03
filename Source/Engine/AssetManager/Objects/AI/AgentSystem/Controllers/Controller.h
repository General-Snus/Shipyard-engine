#pragma once
#include "../AIEventManager.h"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/Reflection/ReflectionTemplate.h>
#include <Tools/Utilities/Math.hpp>

enum class eControllerType : int
{
    dummy,
    player,
    event,
    polling,
    count
};

class Controller : public Reflectable
{
  public:
    ReflectableTypeRegistration()
    virtual bool Update(GameObject input) = 0;
    virtual void Recieve(const AIEvent &aEvent);
    virtual bool ComponentRequirement(GameObject input);

  protected:
    eControllerType controllerType;
};

REFL_AUTO(type(Controller))
