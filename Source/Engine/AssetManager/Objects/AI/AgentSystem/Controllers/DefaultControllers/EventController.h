#pragma once
#include "../Controller.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h"
#include "Engine/AssetManager/Objects/AI/NavMesh/NavMesh.h"

class EventController : public Controller
{
  public:
    MYLIB_REFLECTABLE()
    explicit EventController();
    bool Update(GameObject aPosition) override;
    void Recieve(const AIEvent &aEvent) override;

  private:
    bool pathToTarget;
    Vector3f end;
    NavMesh::Path myPath;
};

REFL_AUTO(type(EventController))
