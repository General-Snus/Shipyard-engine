#pragma once

#include <filesystem>
#include <wtypes.h>
#include <Editor/Editor/Core/GameState/GameState.h>
#include <Executable/Executable/Export.h>
#include <Tools/Utilities/System/Event.h>
#include "Engine/AssetManager/ComponentSystem/GameObject.h"

class YourGameLauncher : public GameLauncher
{
	void Init() override;
	void Start() override;
	void Update(float delta) override;
	void SyncServices(ServiceLocator& serviceLocator) override;

	GameObject player;
};

extern "C" {
GAME_API GameLauncher* entrypointMain();
GAME_API void          exitPoint(HMODULE handle);
}
