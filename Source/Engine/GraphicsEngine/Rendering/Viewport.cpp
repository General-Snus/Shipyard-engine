#include "GraphicsEngine.pch.h"
#include "Viewport.h"

#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"

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
			CameraSettings settings;
			settings.APRatio = ViewportResolution.x / ViewportResolution.y;
			camera->SetSettings(settings);
		}
	}
	else
	{
		myVirtualCamera.SetActive(IsSelected());
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
