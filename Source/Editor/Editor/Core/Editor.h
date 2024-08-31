#pragma once
#include "../Windows/SplashWindow.h"
#include <Editor/Editor/Defines.h>
#include <Game/GameLauncher/Core/GameLauncher.h>
#include <Tools/Utilities/DataStructures/Queue.hpp>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>

class Viewport;
class CommandBuffer;
struct ImGuizmoOp;
class EditorWindow;
class ScriptGraphEditor;
class Scene;

enum class EditorCallback
{
    ObjectSelected,
    WM_DropFile,
};

class Editor : public Singleton<Editor>
{

    friend class Singleton<Editor>;
    friend class Logger;
    friend class ContentDirectory;
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
    void DoWinProc(const MSG &msg);

    static RECT GetViewportRECT();
    static Vector2<unsigned int> GetViewportResolution();

    bool GetIsGUIActive() const
    {
        return IsGUIActive;
    };
    void SetIsGUIActive(bool set)
    {
        IsGUIActive = set;
    };
    static inline std::vector<std::shared_ptr<EditorWindow>> g_EditorWindows;

    static std::vector<GameObject> &GetSelectedGameObjects()
    {
        return m_SelectedGameObjects;
    }
    static void CheckSelectedForRemoved();
    static void Copy();
    static void Paste();

    static inline bool IsPlaying = false;

    void FocusObject(const GameObject &focus, bool focusWithOffset = true) const;
    void AlignObject(const GameObject &focus) const;
    static std::shared_ptr<Scene> GetMainScene()
    {
        return m_MainScene;
    }

    std::unordered_map<EditorCallback, Event> m_Callbacks;

  private:
    Editor() = default;
    std::vector<std::filesystem::path> WM_DroppedPath;

    inline static RECT ViewportRect;
    inline static std::vector<GameObject> m_SelectedGameObjects;
    inline static std::vector<GameObject> copiedObjects;
    inline static std::shared_ptr<Scene> m_MainScene;
    std::vector<std::shared_ptr<Viewport>> m_Viewports;
    GameLauncher myGameLauncher;
    bool IsGUIActive = true;
};
