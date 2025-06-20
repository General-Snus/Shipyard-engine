#include "../Viewport.h"
#include "ImGuiHelpers.hpp"
#include "imgui_internal.h"
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "DirectX/XTK/Inc/SimpleMath.h" 
#include <Tools/ImGui/ImGuizmo.h> 
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/PersistentSystems/SceneUtilities.h>
#include <Font/IconsFontAwesome5.h>
#include "Engine/PersistentSystems/Scene.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/Input/Input.hpp"

ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
ImGuizmo::MODE      m_CurrentGizmoMode = ImGuizmo::MODE::WORLD;

Viewport::Viewport(bool IsMainViewPort, Vector2ui ViewportResolution, std::shared_ptr<Scene> aScene,
	std::shared_ptr<TextureHolder> RenderTexture)
	: m_RenderTarget(RenderTexture), ViewportResolution(ViewportResolution), sceneToRender(aScene),
	isGameViewport(IsMainViewPort), editorCamera(0, nullptr)
{
	editorCamera.Init();
	if (!m_RenderTarget)
	{
		m_RenderTarget = std::make_shared<TextureHolder>("ViewportTargetTexture");
	}

	if (m_RenderTarget->GetRawTexture()->GetWidth() != ViewportResolution.x &&
		m_RenderTarget->GetRawTexture()->GetHeight() != ViewportResolution.y)
	{
		auto clearColor = Vector4f(0, 0, 1, 1);
		if (!ColorManagerInstance.GetColor("ClearColor", clearColor))
		{
			ColorManagerInstance.CreateColor("ClearColor", clearColor);
		}
		m_RenderTarget->GetRawTexture()->AllocateTexture(ViewportResolution, "Target1", clearColor);
	}
}

Viewport::~Viewport() = default;

std::shared_ptr<Scene> Viewport::GetAttachedScene() const
{
	return sceneToRender ? sceneToRender : EDITOR_INSTANCE.GetActiveScene();
}

Texture* Viewport::GetTarget() const
{
	return m_RenderTarget->GetRawTexture().get();
}

bool Viewport::IsSelected() const
{
	return isWindowFocused;
}

bool Viewport::IsHovered() const
{
	return IsMouseHoverering;
}

bool Viewport::IsRenderReady() const
{
	// if you are main and there is a active camera
	if (isGameViewport && IsVisible)
	{
		const auto camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>();
		return camera ? camera->IsActive() : false;
	}

	// Return only true if the local camera is valid and it is visible in imgui
	return IsVisible;
}

bool Viewport::IsGameViewport() const
{
	return isGameViewport;
}

void Viewport::Update()
{
	OPTICK_EVENT();
	if (isGameViewport)
	{
		if (auto* camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>())
		{
			camera->IsInControl(EDITOR_INSTANCE.gameState.LauncherIsPlaying());
			camera->SetResolution(ViewportResolution);
		}
	}
	else
	{
		editorCamera.Update();
		editorCamera.IsInControl(IsSelected());
		editorCamera.SetResolution(ViewportResolution);
	}
}

void Viewport::ResolutionUpdate()
{
	
}

Vector2f Viewport::getCursorInWindowPostion() const
{
	return cursorPositionInViewPort;
}

const Camera& Viewport::GetCamera() const
{
	if (isGameViewport)
	{
		if (auto const* camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>())
		{
			return *camera;
		}
		return editorCamera;
	}
	return editorCamera;
}

Camera& Viewport::GetCamera()
{
	if (isGameViewport)
	{
		if (auto* camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>())
		{
			return *camera;
		}
		return editorCamera;
	}
	return editorCamera;
}

Transform& Viewport::GetCameraTransform()
{
	return GetCamera().LocalTransform();
}


const Transform& Viewport::GetCameraTransform() const
{
	return GetCamera().LocalTransform();
}

// ImGui did not like reverse projection so i put it back to 0-1 depth only for imgui
Matrix Viewport::Projection() const
{
	const auto& ViewportCamera = GetCamera();
	const auto dxMatrix = DirectX::XMMatrixPerspectiveFovLH(ViewportCamera.FowInRad(), ViewportResolution.x / ViewportResolution.y,
		ViewportCamera.nearField, ViewportCamera.farfield);
	return Matrix(&dxMatrix);
}

Matrix Viewport::ViewInverse() const
{
	return Matrix::Invert(GetCamera().LocalTransform().unmodified_WorldMatrix());
}

const Matrix& Viewport::View() const
{
	return GetCamera().LocalTransform().unmodified_WorldMatrix();
}

void Viewport::RenderImGUi()
{
	OPTICK_EVENT();
	constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;
	// const auto aspecRatio = (res.x / res.y);
	// ImGui::SetNextWindowSizeConstraints(ImVec2(0,0),ImVec2(FLT_MAX,FLT_MAX),CustomConstraints::AspectRatio,(void*)&aspecRatio);

	const std::vector<GameObject>& selectedObjects = EDITOR_INSTANCE.GetSelectedGameObjects();

	TakeInput();

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.f, 0.f});
	std::string title = "Scene ";
	if (IsGameViewport())
	{
		title = "Game";
	}
	else
	{
		title += std::to_string(ViewportIndex);
	}

	if (ImGui::Begin(title.c_str(), &m_KeepWindow, windowFlags))
	{  
		isWindowFocused = ImGui::IsWindowFocused();
		IsVisible = ImGui::IsItemVisible(); 
		ViewportResolution = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

		auto& ViewportCamera = GetCamera();
		ViewportCamera.SetResolution(ViewportResolution); 
		const auto cursorPosition = ImGui::GetCursorScreenPos();
		ImGui::Image(m_RenderTarget, *(ImVec2*)&ViewportResolution.x);
		IsMouseHoverering = isWindowFocused;

		if (IsMouseHoverering)
		{
			cursorPositionInViewPort = ImGui::CursorPositionInWindow();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* test = ImGui::AcceptDragDropPayload("ContentAsset_Mesh"))
			{
				const std::filesystem::path data = std::string(static_cast<char*>(test->Data), test->DataSize);
				const std::string           type = ENGINE_RESOURCES.AssetType(data);

				SceneUtils::AddAssetToScene(data, sceneToRender);

				LOGGER.Log(type);
			}
			ImGui::EndDragDropTarget();
		}

		if (!IsGameViewport())
		{
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(cursorPosition.x, cursorPosition.y, ViewportResolution.x, ViewportResolution.y);
			auto cameraView = ViewInverse();
			auto cameraProjection = Projection();

			if (!isWindowFocused)
			{
				GetCamera().IsInControl(false);
			}

			for (auto& gameObject : selectedObjects)
			{
				auto& transform = gameObject.transform();
				Matrix worldMatrix = transform.WorldMatrix();
				Matrix deltaChange;
				float snapping = 0.0f;
				const bool transformed = ImGuizmo::Manipulate(&cameraView, &cameraProjection, m_CurrentGizmoOperation,
					m_CurrentGizmoMode, &worldMatrix, &deltaChange, &snapping);

				if (transformed)
				{
					GetCamera().IsInControl(false);
					Matrix localMatrix = worldMatrix;
					if (transform.HasParent())
					{
						localMatrix = worldMatrix * Matrix::Invert(transform.GetParent().WorldMatrix());
						// TODO This doesnt support scaled objects, fix asap im eepy now
					}

					Vector3f tra;
					Vector3f rot;
					Vector3f sca;
					//ImGuizmo::DecomposeMatrixToComponents(&localMatrix,&tra.x,&rot.x,&sca.x);
					ImGuizmo::DecomposeMatrixToComponents(&deltaChange, &tra.x, &rot.x, &sca.x);

					transform.Scale(sca);
					transform.Rotate(rot);
					transform.Move(tra);

					LOGGER.Log(localMatrix.rotationMatrix().toString());

					transform.DecomposeMatrixToTransform(localMatrix);


				}
				else
				{
					if (isWindowFocused)
					{
						GetCamera().IsInControl(true);
					}
				}
			}

			//RenderToolbar();
		}
	}
	else
	{
		isWindowFocused = false;
		IsVisible = false;
	}
	ImGui::End();
	//ImGui::PopStyleVar();
}

void Viewport::RenderToolbar()
{
	const auto& style = ImGui::GetStyle();
	const float textHeight = ImGui::CalcTextSize("A").y;
	// style.FramePadding can also be used here
	const auto toolbarItemSize = ImVec2{ textHeight * 4.0f, textHeight * 4.0f };
	const auto toolbarPos = ImVec2(
		2.0f * style.WindowPadding.x + ImGui::GetWindowPos().x,
		8.0f * (style.WindowPadding.y + style.FramePadding.y) + ImGui::GetWindowPos().y
	);

	ImGui::SetNextWindowPos(toolbarPos);

	// ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
	//                                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
	//                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

	constexpr ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_NoPadWithHalfSpacing;
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
