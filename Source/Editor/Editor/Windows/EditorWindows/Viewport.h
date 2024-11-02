#pragma once
#include "DirectX/DX12/Graphics/Texture.h"
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

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
                      std::shared_ptr<Scene> sceneToRender = nullptr,
                      std::shared_ptr<TextureHolder> RenderTexture = nullptr);

    ~Viewport() override;
    bool IsSelected() const;
    bool IsHovered() const;
    bool IsRenderReady();
    bool IsMainViewport() const;
    void Update();
    void ResolutionUpdate();

    std::shared_ptr<Scene> GetAttachedScene();
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

  private:
    std::shared_ptr<Scene> sceneToRender;
    void TakeInput();
    void RenderToolbar();

    bool IsMainViewPort = false;
    bool IsVisible = true;
    bool IsUsed = false;
    bool IsMouseHoverering = false;
    Camera editorCamera;
};
