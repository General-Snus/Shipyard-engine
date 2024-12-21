#include "GameState.h"
#include <Windows.h>

#include <libloaderapi.h>
#include <Editor/Editor/Core/Editor.h>
#include <Engine/PersistentSystems/Scene.h>
#include <Tools/Logging/Logging.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>

GameState::GameState() = default;

void GameState::Intialize(std::filesystem::path aPathToProjectFolder) {
	pathToProjectFolder = aPathToProjectFolder;
	m_EditorBackupScene = EDITOR_INSTANCE.GetActiveScene();
}

bool GameState::AttemptDllLoad() {
	dllHandle = LoadLibrary(L"GameLauncher.dll");
	if(!dllHandle) {
		LOGGER.Err("Failed to load DLL!");
		return false;
	}

	dllFunction = reinterpret_cast<EntryPoint>(GetProcAddress(static_cast<HMODULE>(dllHandle),"entrypointMain"));
	dllFunctionExit = reinterpret_cast<ExitPoint>(GetProcAddress(static_cast<HMODULE>(dllHandle),"exitPoint"));
	if(!(dllFunction && dllFunctionExit)) {
		LOGGER.Err("Failed to get DLL function !");
		FreeLibrary(static_cast<HMODULE>(dllHandle));
		return false;
	}

	m_GameLauncher = dllFunction();
	return true;
}

void GameState::StartPlaySession() {
	if(IsLoading || IsPlaying || IsPaused) {
		IsPaused = false;
		IsPlaying = true;
		return;
	}

	m_GameScene = std::make_shared<Scene>("GameScene");

	m_EditorBackupScene = EDITOR_INSTANCE.GetActiveScene();
	m_GameScene->merge(*m_EditorBackupScene);
	EDITOR_INSTANCE.SetActiveScene(m_GameScene);

	// If you start here, all the copied gameobjects now point to the wrong scene and causes issue
	IsLoading = true;
	AttemptDllLoad();
	IsPlaying = true;
}

void GameState::EndPlaySession() {
	EDITOR_INSTANCE.SetActiveScene(m_EditorBackupScene);

	if(m_GameScene) {
		m_GameScene->unload(); 
	}

	if(dllFunction) {
		m_GameScene = std::make_shared<Scene>("GameScene");
		try {
			LOGGER.Err("Failed to get DLL function !");
			dllFunction = nullptr;
		} catch(const std::exception& e) {
			LOGGER.Critical(e,0);
		}
	}


	Shipyard_PhysXInstance.resetScene();
	if(!IsPlaying) {
		return;
	}

	IsPlaying = false;
	IsPaused = false;
}

void GameState::PausePlaySession() {
	IsPaused = true;
}

void GameState::Init() {
	try {
		if(m_GameLauncher) {
			m_GameLauncher->SyncServices(ServiceLocator::Instance());
			m_GameLauncher->Init();
		}
	} catch(const std::exception& e) {
		//IsLoading = false;
		//IsPlaying = false;
		LOGGER.Err(e.what());
	}
}

void GameState::Start() {
	try {
		if(m_GameLauncher) {
			m_GameLauncher->Start();
		}
	} catch(const std::exception& e) {
		//IsLoading = false;
		//IsPlaying = false;
		LOGGER.Err(e.what());
	}
}

void GameState::Update(float delta) {
	try {
		if(IsLoading) {
			IsLoading = false;
			Init();
			Start();
		}

		if(m_GameLauncher && !IsPaused && IsPlaying) {
			m_GameLauncher->Update(delta);
		}
	} catch(const std::exception& e) {
		//IsLoading = false;
		//IsPlaying = false;
		LOGGER.Err(e.what());
	}
}
