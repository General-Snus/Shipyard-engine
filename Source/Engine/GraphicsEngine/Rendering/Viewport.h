#pragma once
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h> 
#include "DirectX/Shipyard/Texture.h"
#include "Engine/AssetManager/ComponentSystem/GameObject.h"

class cCamera;
class Transform;


class Viewport
{
	//New rule! Always give reason for friending in comment
	//Reason: ImGui Only knows the bounding and status of the viewport when rendering so all configuration here will be one frame delayed
	friend class GraphicsEngine;
public:
	//MainViewport is will render from the MainCamera and if no such camera exist it will be black
	Viewport(bool IsMainViewPort);
	bool IsSelected();
	bool IsRenderReady();
	bool IsMainViewport();
	void Update();

	cCamera& GetCamera();
	Transform& GetCameraTransform();

	Matrix Projection();
	Matrix ViewInverse();
	Texture myRenderTexture;
	Vector2f ViewportResolution;
	int ViewportIndex = 0;
private:
	bool IsMainViewPort = false;
	bool IsVisible = true;
	bool IsUsed = false;
	GameObject myVirtualCamera;
};

