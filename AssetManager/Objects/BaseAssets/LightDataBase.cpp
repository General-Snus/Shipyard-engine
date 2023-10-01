#include "AssetManager.pch.h"
#include "LightDataBase.h"
#include <ThirdParty/CU/Math.hpp> 
#include <Modelviewer/Core/Modelviewer.h>




void DirectionalLight::CalculateDirectionLight(Vector3f direction)
{
	const float radius = ModelViewer::Get().GetWorldBounds().GetRadius();
	Vector3f lightPosition = radius * 2.0f * -direction;
	const Vector3f worldCenter = ModelViewer::Get().GetWorldBounds().GetCenter();
	direction = (lightPosition - worldCenter).GetNormalized();
	this->lightView = CU::Matrix4x4<float>::LookAt(lightPosition,ModelViewer::Get().GetWorldBounds().GetCenter(),{0,1,0}); // REFACTOR, Magic value up

	const float leftPlane = lightPosition.x - radius;
	const float bottomPlane = lightPosition.y - radius;
	const float nearPlane = 1;
	const float rightPlane = lightPosition.x + radius;
	const float topPlane = lightPosition.y + radius;
	const float farPlane = radius * 10;
	projection = Matrix::CreateOrthographicProjection(
		leftPlane,
		bottomPlane,
		nearPlane,
		rightPlane,
		topPlane,
		farPlane);
}