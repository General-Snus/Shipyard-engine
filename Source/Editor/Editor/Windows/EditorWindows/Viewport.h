#pragma once
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>  
#include "DirectX/Shipyard/Texture.h"
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Editor/Editor/Core/Editor.h"
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
		Vector2f ViewportResolution = { (float)Window::Width(),(float)Window::Height() },
		std::shared_ptr<Scene> sceneToRender = Editor::GetMainScene(),
		std::shared_ptr<TextureHolder> RenderTexture = nullptr
	);
 
	~Viewport() override;
	bool IsSelected() const;
	bool IsRenderReady();
	bool IsMainViewport() const;
	void Update();
	Texture* GetTarget() const;

	cCamera& GetCamera();
	Transform& GetCameraTransform();

	Matrix Projection();
	Matrix ViewInverse();
	Matrix& View();
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
	GameObject myVirtualCamera;
};

