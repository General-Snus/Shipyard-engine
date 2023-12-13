#pragma once
#include <vector>
#include "ApplicationState.h"
#include "../Windows/SplashWindow.h" 
#include <Tools/Logging/Logging.h> 
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Game/GameLauncher/Core/GameLauncher.h>
#include <Editor/Editor/Defines.h>
class GameLauncher;

class Editor
{
	enum eMenuLayers
	{
		count
	};
private:
	SplashWindow* mySplashWindow = nullptr;
	ApplicationState myApplicationState;
	Logger MVLogger;
	void ShowSplashScreen();
	void HideSplashScreen() const;

	void UpdateImGui();
	void Update();
	void Render();

#pragma region IMGUI
	void TopBar(); 
#pragma endregion


public:
	Editor() = default;
	bool Initialize(HWND aHandle);
	void DoWinProc(const MSG& msg);
	int Run();


	// Acceleration Getters for components.
	static ApplicationState& GetApplicationState()
	{
		static ApplicationState myApplicationState;
		return myApplicationState;
	}

	static Editor& GetEditor() // HATE
	{
		static Editor aEditorInstance;
		return aEditorInstance;
	}
	FORCEINLINE Logger& GetLogger()
	{
		return MVLogger;
	}

	static RECT GetViewportRECT();
	static Vector2<int> GetViewportResolution();

	void ExpandWorldBounds(Sphere<float> sphere);
	const Sphere<float>& GetWorldBounds();

	const bool GetIsGUIActive() const { return IsGUIActive;	};
	void SetIsGUIActive(bool set) { IsGUIActive = set; };
private:
#if WorkingOnMultiThread
	std::thread myLogicThread;
	std::mutex myMutex;
	std::condition_variable myThreadSync;
	std::atomic_bool myIsUpdating = false;
	std::atomic_bool myIsRendering = false;
	std::atomic_bool myIsRunning = false;
#endif


	Sphere<float> myWorldBounds;
	GameLauncher myGameLauncher;
	bool IsGUIActive = true;
	std::array<bool,count> activeWindows = {false};
};
