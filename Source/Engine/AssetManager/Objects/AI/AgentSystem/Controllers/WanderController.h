#pragma once
#include "Controller.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h"

class WanderController : public Controller
{
public:
	ReflectableTypeRegistration()
    explicit WanderController(GameObject componentCheck);
    bool Update(GameObject input) override;
    void Recieve(const AIEvent &aEvent) override;

  private:
};

REFL_AUTO(type(WanderController))
