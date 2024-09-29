#include "../Viewport.h"
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "ImGuiHelpers.hpp"
#include "imgui_internal.h"

#include <Tools/ImGui/ImGui/ImGuizmo.h>

#include "Engine/PersistentSystems/Scene.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/Input/Input.hpp"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/PersistentSystems/SceneUtilities.h>
#include <Font/IconsFontAwesome5.h>

ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::MODE::WORLD;

Viewport::Viewport(bool IsMainViewPort, Vector2f ViewportResolution, std::shared_ptr<Scene> aScene,
                   std::shared_ptr<TextureHolder> RenderTexture)
    : m_RenderTarget(RenderTexture), ViewportResolution(ViewportResolution), sceneToRender(aScene),
      IsMainViewPort(IsMainViewPort)
{

    const auto resolution = static_cast<Vector2ui>(ViewportResolution);
    if (!m_RenderTarget)
    {
        m_RenderTarget = std::make_shared<TextureHolder>("ViewportTargetTexture");
    }

    if (!m_RenderTarget->GetRawTexture() || m_RenderTarget->GetRawTexture()->GetWidth() != resolution.x &&
                                                m_RenderTarget->GetRawTexture()->GetHeight() != resolution.y)
    {
        m_RenderTarget->GetRawTexture()->AllocateTexture(resolution, "Target1");
    }

    if (IsMainViewPort)
    {
        myVirtualCamera = GameObject::Create("MainCamera", sceneToRender);
    }
    else
    {
        myVirtualCamera = GameObject::Create("SecondaryCamera", sceneToRender);
        sceneToRender->GetGOM().SetIsVisibleInHierarchy(myVirtualCamera, false);

        auto &camera = myVirtualCamera.AddComponent<Camera>();
        camera.SetResolution(ViewportResolution);
    }
}

Viewport::~Viewport()
{
    if (!IsMainViewPort)
    {
        sceneToRender->GetGOM().DeleteGameObject(myVirtualCamera, true);
    }
}

Texture *Viewport::GetTarget() const
{
    return m_RenderTarget->GetRawTexture().get();
}

bool Viewport::IsSelected() const
{
    return IsUsed;
}

bool Viewport::IsHovered() const
{
    return IsMouseHoverering;
}

bool Viewport::IsRenderReady()
{
    // if you are main and there is a active camera
    if (IsMainViewPort)
    {
        auto camera = myVirtualCamera.TryGetComponent<Camera>();
        return camera ? camera->IsActive() : false;
    }

    // Return only true if the local camera is valid and it is visible in imgui
    return IsVisible;
}

bool Viewport::IsMainViewport() const
{
    return IsMainViewPort;
}

void Viewport::Update()
{
    OPTICK_EVENT();
    if (IsMainViewPort)
    {
        myVirtualCamera = sceneToRender->GetGOM().GetCamera();
        if (auto *camera = myVirtualCamera.TryGetComponent<Camera>())
        {
            camera->IsInControl(EditorInstance.IsPlaying);
            camera->SetResolution(ViewportResolution);
        }
    }
    else
    {
        // myVirtualCamera.SetActive(IsSelected());
        auto &camera = myVirtualCamera.GetComponent<Camera>();
        camera.IsInControl(IsSelected());
        camera.SetResolution(ViewportResolution);
    }
}

void Viewport::ResolutionUpdate()
{
    // m_RenderTarget->GetRawTexture()->AllocateTexture(resolution, "Target1");
}

Camera &Viewport::GetCamera()
{
    return myVirtualCamera.GetComponent<Camera>();
}

Transform &Viewport::GetCameraTransform()
{
    return myVirtualCamera.GetComponent<Transform>();
}

// ImGui did not like reverse projection so i put it back to 0-1 depth only for imgui
Matrix Viewport::Projection()
{
    auto ViewportCamera = myVirtualCamera.TryGetComponent<Camera>();
    if (!ViewportCamera)
    {
        const auto dxMatrix = XMMatrixPerspectiveFovLH(90.f, 16.f / 9, 0.01f, 1000000.0f);
        return Matrix(&dxMatrix);
    }

    const auto dxMatrix = XMMatrixPerspectiveFovLH(ViewportCamera->FowInRad(), ViewportCamera->APRatio(),
                                                   ViewportCamera->nearField, ViewportCamera->farfield);
    return Matrix(&dxMatrix);
}

Matrix Viewport::ViewInverse()
{
    auto ViewportCameraTransform = myVirtualCamera.TryGetComponent<Transform>();
    if (!ViewportCameraTransform)
    {
        return Matrix();
    }
    return ViewportCameraTransform->GetMutableTransform().GetInverse();
}

Matrix &Viewport::View()
{
    auto ViewportCameraTransform = myVirtualCamera.TryGetComponent<Transform>();
    if (!ViewportCameraTransform)
    {
        static Matrix noReturn;
        return noReturn;
    }
    return ViewportCameraTransform->GetMutableTransform();
}

void Viewport::RenderImGUi()
{
    OPTICK_EVENT();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;
    // const auto aspecRatio = (res.x / res.y);
    // ImGui::SetNextWindowSizeConstraints(ImVec2(0,0),ImVec2(FLT_MAX,FLT_MAX),CustomConstraints::AspectRatio,(void*)&aspecRatio);
    // // Aspect ratio

    std::vector<GameObject> const &selectedObjects = EditorInstance.GetSelectedGameObjects();

    TakeInput();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
    std::string title = "Scene ";
    if (IsMainViewport())
    {
        title = "Game";
    }
    else
    {
        title += std::to_string(ViewportIndex);
    }

    if (ImGui::Begin(title.c_str(), &m_KeepWindow, windowFlags))
    {
        IsUsed = ImGui::IsWindowFocused();
        IsVisible = ImGui::IsItemVisible();

        float windowWidth = ImGui::GetContentRegionAvail().x;
        float windowHeight = ImGui::GetContentRegionAvail().y;
        ImGui::Image(m_RenderTarget, ImGui::GetContentRegionAvail());
        const bool isWindowFocused = ImGui::IsItemHovered();
        IsMouseHoverering = isWindowFocused;

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *test = ImGui::AcceptDragDropPayload("ContentAsset_Mesh"))
            {
                const std::filesystem::path data = std::string((char *)test->Data, test->DataSize);
                const std::string type = AssetManagerInstance.AssetType(data);

                SceneUtils::AddAssetToScene(data, sceneToRender);

                Logger.Log(type);
            }
            ImGui::EndDragDropTarget();
        }

        ViewportResolution = {windowWidth, windowHeight};
        if (!IsMainViewport())
        {
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
            auto cameraView = ViewInverse();
            auto cameraProjection = Projection();

            if (!isWindowFocused)
            {
                GetCamera().IsInControl(false);
            }

            for (auto &gameObject : selectedObjects)
            {
                auto &transform = gameObject.transform();
                Matrix mat = transform.WorldMatrix();

                bool transformed = ImGuizmo::Manipulate(&cameraView, &cameraProjection, m_CurrentGizmoOperation,
                                                        m_CurrentGizmoMode, &mat);

                if (transformed)
                {
                    GetCamera().IsInControl(false);
                    Vector3f loc;
                    Vector3f rot;
                    Vector3f scale;

                    if (transform.HasParent())
                    {
                        mat = mat * transform.GetParent()
                                        .WorldMatrix()
                                        .GetInverse(); // TODO This doesnt support scaled objects, fix asap im eepy now
                    }

                    ImGuizmo::DecomposeMatrixToComponents(mat.GetMatrixPtr(), &loc.x, &rot.x, &scale.x);

                    transform.SetPosition(loc);
                    transform.SetRotation(rot);
                    transform.SetScale(scale);
                    transform.Update();
                }
                else
                {
                    if (isWindowFocused)
                    {
                        GetCamera().IsInControl(true);
                    }
                }
            }

            RenderToolbar();
        }
    }
    else
    {
        IsUsed = false;
        IsVisible = false;
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void Viewport::RenderToolbar()
{
    auto const &style = ImGui::GetStyle();
    float textHeight = ImGui::CalcTextSize("A").y;
    // style.FramePadding can also be used here
    ImVec2 toolbarItemSize = ImVec2{textHeight * 4.0f, textHeight * 4.0f};
    ImVec2 toolbarPos = ImGui::GetWindowPos() +
                        ImVec2(2.0f * style.WindowPadding.x, 8.0f * (style.WindowPadding.y + style.FramePadding.y));
    ImGui::SetNextWindowPos(toolbarPos);

    // ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
    //                                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
    //                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_NoPadWithHalfSpacing;
    /*
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
    if (ImGui::BeginChild("##ViewportToolbar_Vertical", ImVec2(toolbarItemSize.x, 0), 0, toolbarFlags))
    {
        // Bring the toolbar window always on top.
        ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        if (ImGui::Selectable("Universal", m_CurrentGizmoOperation == ImGuizmo::OPERATION::UNIVERSAL, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
        }
        if (ImGui::Selectable("Move", m_CurrentGizmoOperation == ImGuizmo::OPERATION::TRANSLATE, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        }
        if (ImGui::Selectable("Rotate", m_CurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
        }
        ImGui::Separator();
        if (ImGui::Selectable("Scale", m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        }
        if (ImGui::Selectable("Bounds", m_CurrentGizmoOperation == ImGuizmo::OPERATION::BOUNDS, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;
        }
        std::string mode;
        m_CurrentGizmoMode ? mode = "Global" : mode = "Local";
        if (ImGui::Selectable(mode.c_str(), false, selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoMode = static_cast<ImGuizmo::MODE>(!static_cast<bool>(m_CurrentGizmoMode));
        }
        ImGui::PopStyleVar();
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    */
    if (ImGui::BeginMenuBar())
    {
        // Bring the toolbar window always on top.
        ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        // ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::Text("Gizmo Op\neration");
        static bool arg = true;
        ImGui::ToggleButton("Test", &arg);

        if (ImGui::Selectable(ICON_FA_LOCATION_ARROW, m_CurrentGizmoOperation == ImGuizmo::OPERATION::UNIVERSAL,
                              selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
        }
        // Move
        if (ImGui::Selectable(ICON_FA_ARROWS_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::TRANSLATE,
                              selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        }
        // Rotate
        if (ImGui::Selectable(ICON_FA_SYNC_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
        }
        ImGui::Separator();
        if (ImGui::Selectable(ICON_FA_EXPAND, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, selectableFlags,
                              toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        }
        if (ImGui::Selectable(ICON_FA_GLOBE, false, selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoMode = static_cast<ImGuizmo::MODE>(!static_cast<bool>(m_CurrentGizmoMode));
        }

        // posSnapping
        if (ImGui::Selectable(ICON_FA_BORDER_ALL, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE,
                              selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        }
        // rotSnapping
        if (ImGui::Selectable(std::format("{}{}", ICON_FA_UNDO_ALT, "*").c_str(),
                              m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        }
        // ScaleSnapping
        if (ImGui::Selectable(ICON_FA_EXPAND_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE,
                              selectableFlags, toolbarItemSize))
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        }
        // ImGui::PopStyleVar();
        ImGui::EndMenuBar();
    }
}

void Viewport::TakeInput()
{
    if (Input.IsKeyPressed(Keys::G))
    {
        m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    }

    if (Input.IsKeyPressed(Keys::R))
    {
        m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
    }

    if (Input.IsKeyPressed(Keys::S))
    {
        m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
    }

    if (Input.IsKeyPressed(Keys::B))
    {
        m_CurrentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;
    }

    if (Input.IsKeyPressed(Keys::V))
    {
        m_CurrentGizmoMode = static_cast<ImGuizmo::MODE>(!static_cast<bool>(m_CurrentGizmoMode));
    }

    if (Input.IsKeyPressed(Keys::X))
    {
        if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::TRANSLATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE_X;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::ROTATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE_X;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::SCALE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE_X;
        }
    }

    if (Input.IsKeyPressed(Keys::Y))
    {
        if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::TRANSLATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE_Y;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::ROTATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE_Y;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::SCALE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE_Y;
        }
    }

    if (Input.IsKeyPressed(Keys::Z))
    {
        if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::TRANSLATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE_Z;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::ROTATE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE_Z;
        }
        else if (m_CurrentGizmoOperation & ImGuizmo::OPERATION::SCALE)
        {
            m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE_Z;
        }
    }
}
