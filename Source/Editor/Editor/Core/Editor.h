#pragma once
#include <Editor/Editor/Defines.h>
#include <Game/GameLauncher/Core/GameLauncher.h>
#include <Tools/Utilities/DataStructures/Queue.hpp> 
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include "../Windows/SplashWindow.h"  


class Viewport;
struct ImGuizmoOp;
class EditorWindow;
class ScriptGraphEditor;
class Scene;

enum class EditorCallback
{
	ObjectSelected
};

class Editor : public Singleton<Editor>
{

	friend class Singleton<Editor>;
	friend class Logger;
	std::unique_ptr<SplashWindow> mySplashWindow = nullptr;
	void ShowSplashScreen();
	void HideSplashScreen() const;

	void UpdateImGui();
	void Update();
	void Render();

	void AddViewPort();
	void TopBar();
public:



	int Run();
	bool Initialize(HWND aHandle);
	void DoWinProc(const MSG& msg);

	static RECT GetViewportRECT();
	static Vector2<unsigned int> GetViewportResolution();

	bool GetIsGUIActive() const { return IsGUIActive; };
	void SetIsGUIActive(bool set) { IsGUIActive = set; };
	void AddRenderJob(std::shared_ptr<Viewport> aViewport);
	uint32_t GetAmountOfRenderJob();
	static inline std::vector<std::shared_ptr<EditorWindow>> g_EditorWindows;
	static std::vector<GameObject>& GetSelectedGameObjects() { return m_SelectedGameObjects; }
	static inline bool IsPlaying = false;

	static std::shared_ptr<Scene> GetMainScene()
	{
		return m_MainScene;
	}

	std::unordered_map<EditorCallback,Event> m_Callbacks;


private:
	Editor() = default;
	inline static RECT ViewportRect;
	std::shared_ptr<ScriptGraphEditor> ScriptEditor;
	inline static std::vector<GameObject> m_SelectedGameObjects;
	static inline std::shared_ptr<Scene> m_MainScene;
	std::vector< std::shared_ptr<Viewport>> m_Viewports;
	std::vector< std::shared_ptr<Viewport>> m_CustomSceneRenderPasses; //lifetime 1 frame 
	GameLauncher myGameLauncher;
	bool IsGUIActive = true;
};
