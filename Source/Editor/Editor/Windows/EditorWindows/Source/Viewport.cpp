#include "../Viewport.h" 
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "imgui_internal.h"
#include "ImGuiHepers.hpp" 

#include <Tools/ImGui/ImGui/ImGuizmo.h>

#include "Input/Input.hpp"

ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::MODE::WORLD;

Viewport::Viewport(bool IsMainViewPort) : IsMainViewPort(IsMainViewPort)
{
	myRenderTexture.AllocateTexture({ Window::Width(),Window::Height() },"Target1");
	if (IsMainViewPort)
	{
		myVirtualCamera = GameObjectManager::Get().GetCamera();
	}
	else
	{
		myVirtualCamera = GameObjectManager::Get().CreateGameObject();
		myVirtualCamera.AddComponent<cCamera>();
		myVirtualCamera.AddComponent<Transform>();
	}
}

bool Viewport::IsSelected()
{
	return IsUsed;
}

bool Viewport::IsRenderReady()
{
	//if you are main and there is a active camera
	if (IsMainViewPort)
	{
		auto camera = myVirtualCamera.TryGetComponent<cCamera>();
		return camera ? camera->IsActive() : false;
	}

	//Return only true if the local camera is valid and it is visible in imgui
	return IsVisible;
}

bool Viewport::IsMainViewport()
{
	return IsMainViewPort;
}

void Viewport::Update()
{
	if (IsMainViewPort)
	{
		myVirtualCamera = GameObjectManager::Get().GetCamera();
		if (auto* camera = myVirtualCamera.TryGetComponent<cCamera>())
		{
			camera->GetSettings().APRatio = ViewportResolution.x / ViewportResolution.y;
			camera->GetSettings().IsInControll = Editor::IsPlaying;
		}
	}
	else
	{
		myVirtualCamera.SetActive(IsSelected());
		myVirtualCamera.GetComponent<cCamera>().GetSettings().IsInControll = IsSelected();
	}
}

cCamera& Viewport::GetCamera()
{
	return myVirtualCamera.GetComponent<cCamera>();
}

Transform& Viewport::GetCameraTransform()
{
	return myVirtualCamera.GetComponent<Transform>();
}

//ImGui did not like reverse projection so i put it back to 0-1 depth only for imgui
Matrix Viewport::Projection()
{
	auto ViewportCamera = myVirtualCamera.TryGetComponent<cCamera>();
	if (!ViewportCamera)
	{
		CameraSettings settings;
		const auto dxMatrix = XMMatrixPerspectiveFovLH(settings.fow,settings.APRatio,settings.nearField,settings.farfield);
		return Matrix(&dxMatrix);
	}
	auto settings = ViewportCamera->GetSettings();
	const auto dxMatrix = XMMatrixPerspectiveFovLH(settings.fow,settings.APRatio,settings.nearField,settings.farfield);
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

void Viewport::RenderImGUi()
{
	/*ImGuiWindowFlags windowFlags = ImGuiViewportFlags_IsPlatformWindow
		| ImGuiViewportFlags_NoDecoration
		| ImGuiViewportFlags_NoTaskBarIcon
		| ImGuiViewportFlags_NoAutoMerge
		| ImGuiViewportFlags_CanHostOtherWindows
		| ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize;
		//const auto aspecRatio = (res.x / res.y);
		//ImGui::SetNextWindowSizeConstraints(ImVec2(0,0),ImVec2(FLT_MAX,FLT_MAX),CustomConstraints::AspectRatio,(void*)&aspecRatio);   // Aspect ratio
		*/

	std::vector<GameObject>& selectedObjects = Editor::GetSelectedGameObjects();

	if (Input::IsKeyPressed(Keys::G))
	{
		m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}

	if (Input::IsKeyPressed(Keys::R))
	{
		m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}

	if (Input::IsKeyPressed(Keys::S))
	{
		m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	if (Input::IsKeyPressed(Keys::B))
	{
		m_CurrentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;
	}

	if (Input::IsKeyPressed(Keys::V))
	{
		m_CurrentGizmoMode = static_cast<ImGuizmo::MODE>(!static_cast<bool>(m_CurrentGizmoMode));
	}

	if (Input::IsKeyPressed(Keys::X))
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

	if (Input::IsKeyPressed(Keys::Y))
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

	if (Input::IsKeyPressed(Keys::Z))
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



	std::string title = "Scene ";
	if (IsMainViewport())
	{
		title = "Game";
	}
	else
	{
		title += std::to_string(ViewportIndex);

	}

	auto style = ImGui::GetStyle();
	if (ImGui::Begin(title.c_str(),nullptr))
	{
		IsUsed = ImGui::IsWindowFocused();
		IsVisible = ImGui::IsItemVisible();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::Selectable("Open Scene"))
				{

				}
				if (ImGui::Selectable("New Scene"))
				{

				}
				if (ImGui::Selectable("Save Scene"))
				{

				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();
		ImGui::Image(myRenderTexture,ImVec2(windowWidth,windowHeight));

		ViewportResolution = { windowWidth,windowHeight };
		if (!IsMainViewport())
		{
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y,windowWidth,windowHeight);
			auto cameraView = ViewInverse();
			auto cameraProjection = Projection();

			for (auto gameObject : selectedObjects)
			{
				auto& transform = gameObject.GetComponent<Transform>();
				bool transfomed = ImGuizmo::Manipulate(&cameraView,&cameraProjection,m_CurrentGizmoOperation,m_CurrentGizmoMode,
					&transform.GetMutableTransform());


				if (transfomed)
				{
					Vector3f loc;
					Vector3f rot;
					Vector3f scale;
					ImGuizmo::DecomposeMatrixToComponents(
						transform.GetMutableTransform().GetMatrixPtr(),
						&loc,&rot,&scale);

					transform.SetPosition(loc);
					transform.SetRotation(rot);
					transform.SetScale(scale);
				}
			}

			float textHeight = ImGui::CalcTextSize("A").y;
			// style.FramePadding can also be used here
			ImVec2 toolbarItemSize = ImVec2{ textHeight * 4.0f, textHeight * 2.0f };
			ImVec2 toolbarPos = ImGui::GetWindowPos() + ImVec2(2.0f * style.WindowPadding.x,8.0f * style.WindowPadding.y);

			ImGui::SetNextWindowPos(toolbarPos);

			ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDecoration |      //
				ImGuiWindowFlags_NoMove |            //
				ImGuiWindowFlags_NoScrollWithMouse | //
				ImGuiWindowFlags_NoSavedSettings |   //
				ImGuiWindowFlags_NoBringToFrontOnFocus;

			ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_NoPadWithHalfSpacing;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{ 0.0f, 0.0f });


			if (ImGui::BeginChild("##ViewportToolbar",ImVec2(),0,toolbarFlags)) {
				// Bring the toolbar window always on top.
				ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,ImVec2(0.5f,0.5f));

				if (ImGui::Selectable("Universal",m_CurrentGizmoOperation == ImGuizmo::OPERATION::UNIVERSAL,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
				}
				if (ImGui::Selectable("Move",m_CurrentGizmoOperation == ImGuizmo::OPERATION::TRANSLATE,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}
				if (ImGui::Selectable("Rotate",m_CurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
				}
				ImGui::Separator();
				if (ImGui::Selectable("Scale",m_CurrentGizmoOperation == ImGuizmo::OPERATION::SCALE,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				}
				if (ImGui::Selectable("Bounds",m_CurrentGizmoOperation == ImGuizmo::OPERATION::BOUNDS,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;
				}
				std::string mode;
				m_CurrentGizmoMode ? mode = "Global" : mode = "Local";
				if (ImGui::Selectable(mode.c_str(),false,selectableFlags,toolbarItemSize))
				{
					m_CurrentGizmoMode = static_cast<ImGuizmo::MODE>(!static_cast<bool>(m_CurrentGizmoMode));
				}
				ImGui::PopStyleVar();
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	else
	{
		IsUsed = false;
		IsVisible = false;
	}
	ImGui::End();
}
