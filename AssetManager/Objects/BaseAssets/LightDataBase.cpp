#include "AssetManager.pch.h"
#include "LightDataBase.h"
#include <ThirdParty/CU/Math.hpp> 
#include <Modelviewer/Core/Modelviewer.h>




void DirectionalLight::CalculateDirectionLight(Vector3f direction)
{
	const float radius = ModelViewer::Get().GetWorldBounds().GetRadius();
	Vector3f lightPosition = radius * 1.0f * -direction.GetNormalized();
	const Vector3f worldCenter = ModelViewer::Get().GetWorldBounds().GetCenter();
	this->Direction = (worldCenter- lightPosition).GetNormalized(); 
	this->lightView = CU::Matrix4x4<float>::LookAt(lightPosition,worldCenter,{0,1,0}); // REFACTOR, Magic value up
	const Vector4f cameraCenter = Vector4f(lightPosition,0.0f)*this->lightView;
	this->lightView = Matrix::GetFastInverse(this->lightView);

	const float leftPlane = cameraCenter.x - radius*1;
	const float bottomPlane = cameraCenter.y - radius*1;
	const float rightPlane = cameraCenter.x + radius*1;
	const float topPlane = cameraCenter.y + radius*1;
	const float nearPlane = 2.0f;
	const float farPlane = radius * 2;
	projection = Matrix::CreateOrthographicProjection(
		leftPlane,
		rightPlane,
		bottomPlane,
		topPlane,
		nearPlane,
		farPlane);
}