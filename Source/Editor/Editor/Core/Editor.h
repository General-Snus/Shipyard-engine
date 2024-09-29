#pragma once
#include "../Windows/SplashWindow.h"
#include <Editor/Editor/Defines.h>
#include <Game/GameLauncher/Core/GameLauncher.h>
#include <Tools/Utilities/DataStructures/Queue.hpp>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/System/ServiceLocator.h>
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

#define EditorInstance ServiceLocator::Instance().GetService<Editor>()
class Editor : public Singleton
{
    friend class LoggerService;
    friend class ContentDirectory;

    void UpdateImGui();
    void Update();
    void Render();

    void AddViewPort();
    void TopBar();

  public:
    int Run();
    bool Initialize(HWND aHandle);
    void DoWinProc(const MSG &msg);

    RECT GetViewportRECT();
    Vector2<unsigned int> GetViewportResolution();

    bool GetIsGUIActive() const
    {
        return IsGUIActive;
    };
    void SetIsGUIActive(bool set)
    {
        IsGUIActive = set;
    };
    std::vector<std::shared_ptr<EditorWindow>> g_EditorWindows;

    std::vector<GameObject> &GetSelectedGameObjects()
    {
        return m_SelectedGameObjects;
    }
    void CheckSelectedForRemoved();
    void Copy();
    void Paste();

    static inline bool IsPlaying = false;

    void FocusObject(const GameObject &focus, bool focusWithOffset = true) const;
    void AlignObject(const GameObject &focus) const;
    std::shared_ptr<Scene> GetMainScene()
    {
        return m_MainScene;
    }

    std::unordered_map<EditorCallback, Event> m_Callbacks;
    GameState gameState;

  private:
    std::vector<std::filesystem::path> WM_DroppedPath;

    RECT ViewportRect;
    std::vector<GameObject> m_SelectedGameObjects;
    std::vector<GameObject> copiedObjects;
    std::shared_ptr<Scene> m_MainScene;

    std::vector<std::shared_ptr<Viewport>> m_Viewports;
    bool IsGUIActive = true;
};
