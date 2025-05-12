#pragma once  
#include "Editor/Editor/Windows/Window.h"
#include "EditorWindow.h"
#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"  
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

class Transform;
class TextureHolder;
class Texture;

class Viewport : public EditorWindow { 
	friend class Editor; 
public:
	// MainViewport is will render from the MainCamera and if no such camera exist it will be black
	explicit Viewport(bool IsMainViewPort,
		Vector2ui ViewportResolution = WindowInstance.MonitorResolution(),
		std::shared_ptr<Scene> sceneToRender = nullptr,
		std::shared_ptr<TextureHolder> RenderTexture = nullptr);

	~Viewport() override;
	bool IsSelected() const;
	bool IsHovered() const;
	bool IsRenderReady() const;
	bool IsGameViewport() const;
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
	bool isGameViewport = false;
	bool IsVisible = true;
	bool isWindowFocused = false;
	bool IsMouseHoverering = false;
	Camera editorCamera;
};
