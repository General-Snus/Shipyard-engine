#pragma once
#include <Editor/Editor/Core/ApplicationState.h>
#include <Engine/AssetManager/ComponentSystem/Component.h>

class RenderMode : public Component
{
  public:
    MYLIB_REFLECTABLE();
    RenderMode() = delete;
    RenderMode(const SY::UUID anOwnerId, GameObjectManager *aManager) : Component(anOwnerId, aManager) {};
    void Update() override;
    ~RenderMode() override {};

  private:
    int currentlyActiveTone = 0;
    int currentlyActiveLayer = 0;
    bool myRenderMode[(int)DebugFilter::count] = {false};
};
REFL_AUTO(type(RenderMode))
