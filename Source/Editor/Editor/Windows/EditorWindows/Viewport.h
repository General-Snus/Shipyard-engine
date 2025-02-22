#pragma once
#include "DirectX/DX12/Graphics/Resources/Texture.h"
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

class Transform;

class Viewport : public EditorWindow { 
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
	bool IsRenderReady() const;
	bool IsMainViewport() const;
	void Update();
	void ResolutionUpdate();
	Vector2f getCursorInWindowPostion() const;

	std::shared_ptr<Scene> GetAttachedScene() const;
	Texture* GetTarget() const;
	Camera& GetCamera()  ;
	const Camera& GetCamera() const;
	Transform& GetCameraTransform();
	const Transform& GetCameraTransform() const;
	

	Matrix Projection() const;
	Matrix ViewInverse() const;
	const Matrix& View() const;
	void RenderImGUi() override;

	std::shared_ptr<TextureHolder> m_RenderTarget;
	Vector2f ViewportResolution;
	int ViewportIndex = 0;

private:
	std::shared_ptr<Scene> sceneToRender;
	void TakeInput();
	void RenderToolbar();

	Vector2f cursorPositionInViewPort;
	bool IsMainViewPort = false;
	bool IsVisible = true;
	bool isWindowFocused = false;
	bool IsMouseHoverering = false;
	Camera editorCamera;
};
