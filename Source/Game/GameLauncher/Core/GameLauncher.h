#pragma once

#include <filesystem>
#include <wtypes.h>
#include <Editor/Editor/Core/GameState/GameState.h>
#include <Executable/Executable/Export.h>
#include <Tools/Utilities/System/Event.h>
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/PersistentSystems/Networking/NetworkRunner.h"

class YourGameLauncher : public GameLauncher
{
	void Init() override;
	void Start() override;
	void Update(float delta) override;
	void SyncServices(ServiceLocator& serviceLocator) override;

	GameObject player; 
	int amountOfBalls = 10;
	float ballSpawnTimer = 1.0f;
	float ballSpawnCooldown = 1.0f;
	Vector3f rect = {50,1,50};
};

extern "C" {
GAME_API GameLauncher* entrypointMain();
GAME_API void          exitPoint(HMODULE handle);
}
