#pragma once
#include "DirectX/Shipyard/Texture.h"
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

class Camera;
class Transform;

class Viewport : public EditorWindow
{
    // New rule! Always give reason for friending in comment
    // Reason: ImGui Only knows the bounding and status of the viewport when rendering so all configuration here will be
    // one frame delayed
    friend class Editor;

  public:
    // MainViewport is will render from the MainCamera and if no such camera exist it will be black
    explicit Viewport(bool IsMainViewPort,
                      Vector2f ViewportResolution = {(float)WindowInstance.Width(), (float)WindowInstance.Height()},
                      std::shared_ptr<Scene> sceneToRender = EditorInstance.GetMainScene(),
                      std::shared_ptr<TextureHolder> RenderTexture = nullptr);

    ~Viewport() override;
    bool IsSelected() const;
    bool IsHovered() const;
    bool IsRenderReady();
    bool IsMainViewport() const;
    void Update();
    void ResolutionUpdate();
    Texture *GetTarget() const;

    Camera &GetCamera();
    Transform &GetCameraTransform();

    Matrix Projection();
    Matrix ViewInverse();
    Matrix &View();
    void RenderImGUi() override;

    std::shared_ptr<TextureHolder> m_RenderTarget;
    Vector2f ViewportResolution;
    int ViewportIndex = 0;
    std::shared_ptr<Scene> sceneToRender;

  private:
    void TakeInput();
    void RenderToolbar();

    bool IsMainViewPort = false;
    bool IsVisible = true;
    bool IsUsed = false;
    bool IsMouseHoverering = false;
    GameObject myVirtualCamera;
};
