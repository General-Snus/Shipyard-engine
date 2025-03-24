#pragma once
#include "../../Component.h"
#include <Engine/AssetManager/Objects/AI/AgentSystem/Actor.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h>

class cActor : public Component
{
  public:
    ReflectableTypeRegistration();
    cActor(const SY::UUID anOwnerId, GameObjectManager *aManager);
    ~cActor() override = default;
    void Init() override;
    void Update() override;

    bool InspectorView() override;
    Controller *GetController() const
    {
        return controller;
    }
    void SetController(Controller *aController);

  private:
    Controller *controller = nullptr;
};

REFL_AUTO(type(cActor))
