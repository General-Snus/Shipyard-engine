#include "../Viewport.h"

#include "DirectX/XTK/Inc/SimpleMath.h" 
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Helpers/ImGuiHelpers.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Engine/GraphicsEngine/Renderer.h"
#include "Engine/PersistentSystems/Scene.h"
#include "Optick\include\optick.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/Input/Input.hpp"
#include "Tools\ImGui\imgui.h" 
#include "Tools\ImGui\imgui_internal.h" 
#include "Tools\Utilities\LinearAlgebra\Vector2.hpp"
#include "Tools\Utilities\LinearAlgebra\Vector3.hpp"
#include "Tools\Utilities\LinearAlgebra\Vector4.hpp"
#include "Windows\EditorWindows\EditorWindow.h"
#include "algorithm"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/PersistentSystems/SceneUtilities.h>
#include <Font/IconsFontAwesome5.h>
#include <Tools/ImGui/ImGuizmo.h> 
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <vector>

ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
ImGuizmo::MODE      m_CurrentGizmoMode = ImGuizmo::MODE::WORLD;

Viewport::Viewport(
	bool IsMainViewPort,
	Vector2ui ViewportResolution,
	std::shared_ptr<Scene> aScene,
	std::shared_ptr<TextureHolder> RenderTexture) :
	m_RenderTarget(RenderTexture),
	scaledResolution(ViewportResolution),
	backbufferResolution(ViewportResolution),
	sceneToRender(aScene),
	isGameViewport(IsMainViewPort),
	editorCamera(0, nullptr),
	EditorWindow(IsMainViewPort ? "Game" : "Scene", (int)(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
{
}

Viewport::~Viewport() = default;

void Viewport::Initialize()
{
	editorCamera.Init();
	if (!m_RenderTarget)
	{
		m_RenderTarget = std::make_shared<TextureHolder>("ViewportTargetTexture");
	}

	if (m_RenderTarget->GetRawTexture()->GetWidth() != backbufferResolution.x &&
		m_RenderTarget->GetRawTexture()->GetHeight() != backbufferResolution.y)
	{
		auto clearColor = Vector4f(0, 0, 1, 1);
		if (!ColorManagerInstance.GetColor("ClearColor", clearColor))
		{
			ColorManagerInstance.CreateColor("ClearColor", clearColor);
		}
		m_RenderTarget->GetRawTexture()->AllocateTexture(Vector2ui(backbufferResolution), "Target1", clearColor);
	}
}

std::shared_ptr<Scene> Viewport::GetAttachedScene() const
{
	return sceneToRender ? sceneToRender : GetEditor().GetActiveScene();
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
	if (!IsVisible) { return false; }
	if (!m_RenderTarget->GetRawTexture()->IsValid()) { return false; }
	if (!GetAttachedScene()) { return false; }

	// if you are main and there is a active camera
	if (isGameViewport)
	{
		const auto camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>();
		return camera ? camera->IsActive() : false;
	}

	// Return only true if the local camera is valid and it is visible in imgui
	return true;
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
		if (!GetAttachedScene())
		{
			LOGGER.Err("No attached scene to game camera");
			return;
		}
		if (auto* camera = GetAttachedScene()->GetGOM().GetCamera().TryGetComponent<Camera>())
		{
			camera->IsInControl(false);
			camera->SetResolution(scaledResolution);
		}
	}
	else
	{
		editorCamera.Update();
		editorCamera.IsInControl(IsSelected());
		editorCamera.SetResolution(scaledResolution);
		GizmoInput();
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
	if (!GetAttachedScene()) { return editorCamera; }
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
	const auto dxMatrix = DirectX::XMMatrixPerspectiveFovLH(ViewportCamera.FowInRad(), static_cast<float>(scaledResolution.x) / scaledResolution.y,
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

	const std::vector<GameObject>& selectedObjects = GetEditor().GetSelectedGameObjects();
	RenderToolbar();
	isWindowFocused = ImGui::IsWindowFocused();
	IsVisible = ImGui::IsItemVisible();

	//Unsure why Imgui somethimes think available region is negative
	scaledResolution = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	auto& ViewportCamera = GetCamera();
	ViewportCamera.SetResolution(scaledResolution);
	const auto cursorPosition = ImGui::GetCursorScreenPos();
	ImGui::Image(m_RenderTarget, *(ImVec2*)&scaledResolution.x);
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
			const std::string           type = GetEngineResources().AssetType(data);

			SceneUtils::AddAssetToScene(data, sceneToRender);

			LOGGER.Log(type);
		}
		ImGui::EndDragDropTarget();
	}

	if (!IsGameViewport())
	{
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(cursorPosition.x, cursorPosition.y, static_cast<float>(scaledResolution.x), static_cast<float>(scaledResolution.y));
		auto cameraView = ViewInverse();
		auto cameraProjection = Projection();

		if (!isWindowFocused)
		{
			GetCamera().IsInControl(false);
		}

		bool didManipulate = false;
		for (const auto& gameObject : selectedObjects)
		{
			auto& transform = gameObject.transform();
			Matrix worldMatrix = transform.WorldMatrix();
			Matrix deltaChange;
			float snapping = 0.0f;
			const bool transformed = ImGuizmo::Manipulate(&cameraView, &cameraProjection, m_CurrentGizmoOperation,
				m_CurrentGizmoMode, &worldMatrix, &deltaChange, &snapping);

			didManipulate |= transformed;
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
		}

		GetCamera().IsInControl(!didManipulate && isWindowFocused);
	}
	//}
	//else
	//{
	//	isWindowFocused = false;
	//	IsVisible = false;
	//} 
	////ImGui::PopStyleVar();
}

void Viewport::RenderToolbar()
{
	const auto& style = ImGui::GetStyle();
	const auto textRect = ImGui::CalcTextSize(ICON_FA_LOCATION_ARROW);
	// style.FramePadding can also be used here
	const auto toolbarItemSize = ImVec2{ textRect.x, 2 * textRect.y };
	const auto toolbarPos = ImVec2(
		2.0f * style.WindowPadding.x + ImGui::GetWindowPos().x,
		100.0f * (style.WindowPadding.y + style.FramePadding.y) + ImGui::GetWindowPos().y
	);

	ImGui::SetNextWindowPos(toolbarPos);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 4));

	if (ImGui::BeginMenuBar())
	{
		// Bring the toolbar window always on top.
		ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
		//ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable(ICON_FA_LOCATION_ARROW, m_CurrentGizmoOperation == ImGuizmo::OPERATION::UNIVERSAL, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
		}
		// Move
		if (ImGui::Selectable(ICON_FA_ARROWS_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::TRANSLATE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		}
		// Rotate
		if (ImGui::Selectable(ICON_FA_SYNC_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
		}
		if (ImGui::Selectable(ICON_FA_EXPAND, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		ImGui::Spacing();

		constexpr auto globalSpaceName = ICON_FA_GLOBE " Global";
		constexpr auto localSpaceName = ICON_FA_GLOBE " Local";

		auto currentSelected = m_CurrentGizmoMode == ImGuizmo::MODE::WORLD ? globalSpaceName : localSpaceName;
		if (ImGui::BeginCombo("##TransformationSpace", currentSelected, ImGuiComboFlags_WidthFitPreview))
		{
			if (ImGui::Selectable(globalSpaceName))
			{
				m_CurrentGizmoMode = ImGuizmo::MODE::WORLD;
			}


			if (ImGui::Selectable(localSpaceName))
			{
				m_CurrentGizmoMode = ImGuizmo::MODE::LOCAL;
			}

			ImGui::EndCombo();
		}

		// posSnapping
		if (ImGui::Selectable(ICON_FA_BORDER_ALL, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		// rotSnapping
		if (ImGui::Selectable(std::format("{}{}", ICON_FA_UNDO_ALT, "*").c_str(),
			m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		// ScaleSnapping
		if (ImGui::Selectable(ICON_FA_EXPAND_ALT, m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE, 0, toolbarItemSize))
		{
			m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		//ImGui::PopStyleVar();
		ImGui::Text("Gizmo ");
		static bool arg = true;
		if (ImGui::ToggleButton("DebugDrawerEnabler", &arg))
		{
			GetRenderer().debugDrawer.enabled = arg; //TODO fix later

		}
		ImGui::EndMenuBar();
	}
	ImGui::PopStyleVar();
}

void Viewport::GizmoInput()
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
