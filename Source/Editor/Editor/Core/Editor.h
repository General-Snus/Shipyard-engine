#pragma once  
#include <Editor/Editor/Defines.h> 
#include <Game/GameLauncher/Core/GameLauncher.h> 
#include <Tools/Utilities/DataStructures/Queue.hpp>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>   
#include "../Windows/SplashWindow.h"  
class Viewport;
class GameLauncher;
struct ImGuizmoOp;
class EditorWindow;
class ScriptGraphEditor;

class Editor : public Singleton<Editor>
{
	friend class Singleton<Editor>;
	friend class Logger;
	enum eMenuLayers
	{
		count
	};
private:
	std::unique_ptr<SplashWindow> mySplashWindow = nullptr;
	void ShowSplashScreen();
	void HideSplashScreen() const;

	void UpdateImGui();
	void Update();
	void Render();

	void AddViewPort();
	void TopBar();


public:
	Editor() = default;
	bool Initialize(HWND aHandle);
	void DoWinProc(const MSG& msg);
	int Run();
	static RECT GetViewportRECT();
	static Vector2<unsigned int> GetViewportResolution();

	void ExpandWorldBounds(const Sphere<float>& sphere);
	const Sphere<float>& GetWorldBounds() const;

	bool GetIsGUIActive() const { return IsGUIActive; };
	void SetIsGUIActive(bool set) { IsGUIActive = set; };


	static inline std::vector<std::shared_ptr<EditorWindow>> g_EditorWindows;
	static std::vector<GameObject>& GetSelectedGameObjects() { return m_SelectedGameObjects; }
	static inline bool IsPlaying = false;
private:
	inline static RECT ViewportRect;
	std::shared_ptr<ScriptGraphEditor> ScriptEditor;
	inline static std::vector<GameObject> m_SelectedGameObjects;

	std::vector<std::shared_ptr<Viewport>> m_Viewports;

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
};
