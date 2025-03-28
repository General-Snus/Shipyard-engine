#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include <filesystem>
class Scene;
class GameLauncher {
	// If we do it like this we can then make the user have the gamelaunchers
	// override as a main singleton containing game handlers comparable to DontDestroyOnLoad unity scenes
	friend class GameState;

protected:
	GameLauncher() = default;
	virtual void Init() {};
	virtual void Start() {};
	virtual void Update(float delta) {
		(delta);
	};
	virtual void SyncServices(ServiceLocator& serviceLocator) = 0;

private:
};

class GameState {
public:
	GameState();
	void Intialize(std::filesystem::path aPathToProjectFolder);
	void StartPlaySession();
	void EndPlaySession();
	void PausePlaySession();
	void Update(float delta);
	bool ReportErrors() const {
		return HaveErrors;
	}

	GameLauncher* GetGameLauncher() const {
		return m_GameLauncher;
	}

	bool LauncherIsPlaying() const {
		return IsPlaying;
	}

	bool LauncherIsLoading() const {
		return IsLoading;
	}

private:
	void Init();
	void Start();
	bool AttemptDllLoad();
	friend class Editor;
	bool HaveErrors = false;
	bool IsPaused = false;
	bool IsPlaying = false;
	bool IsLoading = false;
	std::shared_ptr<Scene> m_GameScene{};
	std::shared_ptr<Scene> m_EditorBackupScene{};

	GameLauncher* m_GameLauncher{};
	std::filesystem::path pathToProjectFolder{};

	void* dllHandle{};
	typedef GameLauncher* (*EntryPoint)();
	typedef void (*ExitPoint)(void* handle);
	EntryPoint dllFunction{};
	ExitPoint dllFunctionExit{};
};
