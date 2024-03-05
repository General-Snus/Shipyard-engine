#pragma once  
#include <Editor/Editor/Defines.h>
#include <Editor/Editor/Windows/EditorWindows/ChainGraph/GraphTool.h>
#include <Game/GameLauncher/Core/GameLauncher.h>
#include <Tools/Logging/Logging.h> 
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>  
#include "../Windows/SplashWindow.h" 
#include "ApplicationState.h" 

class GameLauncher;

class Editor : public Singleton<Editor>
{
	friend class Singleton<Editor>;
	enum eMenuLayers
	{
		count
	};
private:
	std::unique_ptr<SplashWindow> mySplashWindow = nullptr;
	ApplicationState myApplicationState;
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
	static RECT GetViewportRECT();
	static Vector2<int> GetViewportResolution();

	void ExpandWorldBounds(const Sphere<float>& sphere);
	const Sphere<float>& GetWorldBounds() const;

	bool GetIsGUIActive() const { return IsGUIActive; };
	void SetIsGUIActive(bool set) { IsGUIActive = set; };
private:
	inline static RECT ViewportRect;
	std::shared_ptr<ScriptGraphEditor> ScriptEditor;



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
	std::array<bool,count> activeWindows = { false };
};
