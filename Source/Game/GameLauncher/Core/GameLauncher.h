#pragma once

#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include <Editor/Editor/Core/GameState/GameState.h>
#include <Executable/Executable/Export.h>
#include <Tools/Utilities/System/Event.h>
#include <filesystem>

class YourGameLauncher : public GameLauncher
{
  private:
    void Init() override;
    void Start() override;
    void Update(float delta) override;
    void SyncServices(ServiceLocator &serviceLocator) override;

  private:
    GameObject player;
};

extern "C"
{
    GAME_API GameLauncher *EntrypointMain();
}
