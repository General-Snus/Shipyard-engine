#pragma once
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>  
#include "DirectX/Shipyard/Texture.h"
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"


class cCamera;
class Transform;


class Viewport : public EditorWindow
{
	//New rule! Always give reason for friending in comment
	//Reason: ImGui Only knows the bounding and status of the viewport when rendering so all configuration here will be one frame delayed
	friend class Editor;
public:
	//MainViewport is will render from the MainCamera and if no such camera exist it will be black
	explicit Viewport(bool IsMainViewPort,
		Vector2ui ViewportResolution = { Window::Width(),Window::Height() },
		GameObjectManager& sceneToRender = GameObjectManager::Get(),
		std::shared_ptr<TextureHolder> RenderTexture = nullptr
	);
 
	~Viewport();
	bool IsSelected();
	bool IsRenderReady();
	bool IsMainViewport();
	void Update();
	Texture* GetTarget();

	cCamera& GetCamera();
	Transform& GetCameraTransform();

	Matrix Projection();
	Matrix ViewInverse();
	void RenderImGUi() override;


	std::shared_ptr<TextureHolder> m_RenderTarget;
	Vector2f ViewportResolution;
	int ViewportIndex = 0;
	GameObjectManager& sceneToRender;
private:
	bool IsMainViewPort = false;
	bool IsVisible = true;
	bool IsUsed = false;
	GameObject myVirtualCamera;
};

