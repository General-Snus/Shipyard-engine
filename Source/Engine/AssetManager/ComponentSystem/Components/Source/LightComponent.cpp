#include "Engine/AssetManager/AssetManager.pch.h"
#include "../LightComponent.h"
//#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/Math.hpp> 
#include <Tools/Optick/include/optick.h>

#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/LightDataBase.h"


cLight::cLight(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{
	myLightType = eLightType::uninitialized;
	shadowMap[0] = std::make_shared<Texture>();
	SetIsShadowCaster(true);
}

cLight::cLight(const SY::UUID anOwnerId,GameObjectManager* aManager,const eLightType type) : Component(anOwnerId,aManager),myLightType(type)
{
	shadowMap[0] = std::make_shared<Texture>();
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData = std::make_shared<DirectionalLight>();
		break;
	case Point:
		myPointLightData = std::make_shared<PointLight>();
		for (int i = 0; i < 6; i++)
		{
			shadowMap[i] = std::make_shared<Texture>();
		}
		break;
	case Spot:
		mySpotLightData = std::make_shared<SpotLight>();
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	SetIsShadowCaster(true);
}

eLightType cLight::GetType() const
{
	return myLightType;
}
void cLight::SetType(const eLightType aType)
{
	myLightType = aType;
}

bool cLight::GetIsShadowCaster() const
{
	return isShadowCaster;
}
void cLight::SetIsShadowCaster(const bool active)
{
	OPTICK_EVENT();
	isShadowCaster = active;

	if (active)
	{
		isDirty = true;
		std::wstring name = L"unNamedMap";
		Vector2ui resolution = { 512,512 };
		int mapsToCreate = 0;
		switch (myLightType)
		{
			using enum eLightType;
		case Directional:
			name = L"directionalLight";
			resolution = { 4096,4096 };
			mapsToCreate = 1;
			break;
		case Point:
			name = L"pointLight";
			resolution = { 512,512 };
			mapsToCreate = 6;
			break;
		case Spot:
			name = L"spotLight";
			resolution = { 1024,1024 };
			mapsToCreate = 1;
			break;
		case uninitialized:
			break;
		default:
			break;
		}

		for (int i = 0; i < mapsToCreate; i++)
		{
			shadowMap[i] = std::make_shared<Texture>();

			shadowMap[i]->AllocateTexture(
				resolution,
				name + std::to_wstring(i) + L"_" +
				std::to_wstring(resolution.x) + L"|" +
				std::to_wstring(resolution.y),
				DXGI_FORMAT_D32_FLOAT,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
				D3D12_RESOURCE_STATE_DEPTH_WRITE);

			shadowMap[i]->SetView(ViewType::DSV);
			shadowMap[i]->SetView(ViewType::SRV);

			switch (myLightType)
			{
				using enum eLightType;
			case Directional:
				myDirectionLightData->castShadow = true;
				myDirectionLightData->shadowMapIndex = shadowMap[i]->GetHandle(ViewType::SRV).heapOffset;
				break;
			case Point:
				myPointLightData->castShadow = true;
				myPointLightData->shadowMapIndex[i] = shadowMap[i]->GetHandle(ViewType::SRV).heapOffset;
				break;
			case Spot:
				mySpotLightData->castShadow = true;
				mySpotLightData->shadowMapIndex = shadowMap[i]->GetHandle(ViewType::SRV).heapOffset;
				break;
			case uninitialized:
				break;
			default:
				break;
			}
		}
	}
}

bool cLight::GetIsRendered() const
{
	return isRendered;
}
void cLight::SetIsRendered(const bool aRendered)
{
	isRendered = aRendered;
}

bool cLight::GetIsDirty() const
{
	return isDirty;
}
void cLight::SetIsDirty(const bool dirty)
{
	isDirty = dirty;
	if (isShadowCaster)
	{
		SetIsRendered(false);
	}
}

void cLight::SetPower(const float power)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Power = power;
		break;
	case Point:
		myPointLightData->Power = power;
		break;
	case Spot:
		mySpotLightData->Power = power;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetPower()const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return myDirectionLightData->Power;
		break;
	case Point:
		return myPointLightData->Power;
		break;
	case Spot:
		return mySpotLightData->Power;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return 0;
}

void cLight::SetColor(const Vector3f color)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Color = color;
		break;
	case Point:
		myPointLightData->Color = color;
		break;
	case Spot:
		mySpotLightData->Color = color;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetColor()const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return myDirectionLightData->Color;
		break;
	case Point:
		return myPointLightData->Color;
		break;
	case Spot:
		return mySpotLightData->Color;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return Vector3f();
}

void cLight::SetPosition(const Vector3f position)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		break;
	case Point:
		myPointLightData->Position = position;
		break;
	case Spot:
		mySpotLightData->Position = position;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetPosition()const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return Vector3f();
		break;
	case Point:
		return myPointLightData->Position;
		break;
	case Spot:
		return mySpotLightData->Position;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return Vector3f();
}

void cLight::SetDirection(const Vector3f direction)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Direction = Vector4f(direction,1);
		break;
	case Point:
		break;
	case Spot:
		mySpotLightData->Direction = direction;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetDirection() const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return Vector3f(
			myDirectionLightData->Direction.x,
			myDirectionLightData->Direction.y,
			myDirectionLightData->Direction.z
		);
		break;
	case Point:
		return Vector3f();
		break;
	case Spot:
		return mySpotLightData->Direction;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return Vector3f();
}

void cLight::SetRange(const float range)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		break;
	case Point:
		myPointLightData->Range = range;
		break;
	case Spot:
		mySpotLightData->Range = range;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetRange() const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return 0;
		break;
	case Point:
		return myPointLightData->Range;
		break;
	case Spot:
		return mySpotLightData->Range;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return 0;
}

void cLight::SetInnerAngle(const float angle)
{

	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		break;
	case Point:
		break;
	case Spot:
		mySpotLightData->InnerConeAngle = angle;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetInnerAngle()const
{
	return 0.0f;
}

void cLight::SetOuterAngle(const float angle)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		break;
	case Point:
		break;
	case Spot:
		mySpotLightData->OuterConeAngle = angle;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetOuterAngle()const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return 0.0f;
		break;
	case Point:
		return 0.0f;
		break;
	case Spot:
		return mySpotLightData->OuterConeAngle;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return 0.0f;
}

std::shared_ptr<Texture> cLight::GetShadowMap(const int number) const
{
	return shadowMap[number];
}

void cLight::Update()
{
	OPTICK_EVENT();
	if (boundToTransform)
	{
		ConformToTransform();
	}
	if (isDirty)
	{
		switch (myLightType)
		{
			using enum eLightType;
		case Directional:
			RedrawDirectionMap();
			myDirectionLightData->Power = std::max(0.0f,myDirectionLightData->Power);
			break;
		case Point:
			RedrawPointMap();
			myPointLightData->Power = std::max(0.0f,myPointLightData->Power);
			myPointLightData->Range = std::max(0.0f,myPointLightData->Range);
			break;
		case Spot:
			RedrawSpotMap();
			mySpotLightData->InnerConeAngle = std::clamp(mySpotLightData->InnerConeAngle,0.f,mySpotLightData->OuterConeAngle);
			mySpotLightData->OuterConeAngle = std::clamp(mySpotLightData->OuterConeAngle,mySpotLightData->InnerConeAngle,360.f);
			mySpotLightData->Power = std::max(0.0f,mySpotLightData->Power);
			mySpotLightData->Range = std::max(0.0f,mySpotLightData->Range);
			break;
		case uninitialized:
			break;
		default:
			break;
		}
		SetIsDirty(false);
	}

}

void cLight::ConformToTransform()
{
	OPTICK_EVENT();
	Transform* transform = this->TryGetComponent<Transform>();
	if (transform == nullptr)
	{
		std::cout << "Light component has no transform component" << std::endl;
		return;
	}
	if (transform->GetIsDirty())
	{
		SetIsDirty(true);
	}
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Direction = Vector4f(transform->GetForward(),1);
		break;
	case Point:
		myPointLightData->Position = transform->GetPosition();
		break;
	case Spot:
		mySpotLightData->Position = transform->GetPosition();
		mySpotLightData->Direction = transform->GetForward();
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}

void cLight::RedrawShadowMap()
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		RedrawDirectionMap();
		break;
	case Point:
		break;
	case Spot:
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}

void cLight::RedrawDirectionMap()
{
	OPTICK_EVENT();
	constexpr float radius = 50;
	myDirectionLightData->Direction.Normalize();

	const Vector3f worldCenter = {};
	const Vector3f lightPosition = radius * 5.0f * -Vector3f(myDirectionLightData->Direction.x,myDirectionLightData->Direction.y,myDirectionLightData->Direction.z);
	myDirectionLightData->lightView = Matrix::LookAt(lightPosition,worldCenter,{ 0,1,0 }); // REFACTOR, Magic value up

	const Vector4f cameraCenter = Vector4f(worldCenter,0.0f) * myDirectionLightData->lightView;
	myDirectionLightData->lightView = Matrix::GetFastInverse(myDirectionLightData->lightView);

	const float leftPlane = cameraCenter.x - radius * 1;
	const float rightPlane = cameraCenter.x + radius * 1;
	const float bottomPlane = cameraCenter.y - radius * 1;
	const float topPlane = cameraCenter.y + radius * 1;
	constexpr float nearPlane = 0.1f;
	constexpr float farPlane = radius * 5.f;
	myDirectionLightData->projection = Matrix::CreateOrthographicProjection(
		leftPlane,
		rightPlane,
		bottomPlane,
		topPlane,
		farPlane,
		nearPlane
	);
}

void cLight::RedrawPointMap()
{
	OPTICK_EVENT();
	constexpr float fow = 90.0f * DEG_TO_RAD;
	constexpr float nearField = .01f;
	const float farfield = std::max(myPointLightData->Range * 5,nearField + 0.0001f);

	const auto dxMatrix = XMMatrixPerspectiveFovLH(fow,1,farfield,nearField);
	myPointLightData->projection = Matrix(&dxMatrix);

	for (int i = 0; i < 6; i++)
	{
		myPointLightData->lightView[i] = GetLightViewMatrix(i);
	}

}

void cLight::RedrawSpotMap()
{
	OPTICK_EVENT();
	Vector3f lightPosition = mySpotLightData->Position;
	mySpotLightData->lightView = Matrix::LookAt(lightPosition,lightPosition + mySpotLightData->Direction.GetNormalized(),{ 0,1,0 }); // REFACTOR, Magic value up

	const float fow = mySpotLightData->OuterConeAngle;
	const float farfield = mySpotLightData->Range * 2;
	constexpr float nearField = 0.01f;


	const auto dxMatrix = XMMatrixPerspectiveFovLH(fow * DEG_TO_RAD,1,farfield,nearField);
	mySpotLightData->projection = Matrix(&dxMatrix);
	mySpotLightData->lightView = Matrix::GetFastInverse(mySpotLightData->lightView);
}

void cLight::BindDirectionToTransform(const bool active)
{
	SetIsDirty(true);
	boundToTransform = active;
}

Matrix cLight::GetLightViewMatrix(const int number) const
{
	Vector3f lightPos = myPointLightData->Position;
	assert(number < 6 && "There are only 6 faces to a cubemap");
	assert(myLightType == eLightType::Point && "Use only for point lights");
	switch (number)
	{
	case 0:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(1.0f,0.0f,0.0f),Vector3f(0.0f,1.0f,0.0f)));
		break;
	case 1:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(-1.0f,0.0f,0.0f),Vector3f(0.0f,1.0f,0.0f)));
		break;
	case 2:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(0.0f,1.0f,0.0f),Vector3f(0.0f,0.0f,1.0f)));
		break;
	case 3:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(0.0f,-1.0f,0.0f),Vector3f(0.0f,0.0f,1.0f)));
		break;
	case 4:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(0.0f,0.0f,1.0f),Vector3f(0.0f,1.0f,0.0f)));
		break;
	case 5:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(0.0f,0.0f,-1.0f),Vector3f(0.0f,1.0f,0.0f)));
		break;
	default:
		return Matrix::GetFastInverse(Matrix::LookAt(lightPos,lightPos + Vector3f(1.0f,0.0f,0.0f),Vector3f(0.0f,1.0f,0.0f)));
		break;
	}
}

bool cLight::GetIsBound()const
{
	return boundToTransform;
}

FrameBuffer cLight::GetShadowMapFrameBuffer(const int number) const
{
	FrameBuffer fb;

	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		fb.ProjectionMatrix = myDirectionLightData->projection;
		fb.ViewMatrix = myDirectionLightData->lightView;
		fb.FB_ScreenResolution = { shadowMap[0]->GetWidth(),shadowMap[0]->GetHeight() };
		fb.FB_CameraPosition = GetPosition();
		break;
	case Point:
		fb.ProjectionMatrix = myPointLightData->projection;
		fb.ViewMatrix = GetLightViewMatrix(number);
		fb.FB_ScreenResolution = { shadowMap[number]->GetWidth(),shadowMap[number]->GetHeight() };
		fb.FB_CameraPosition = GetPosition();
		break;
	case Spot:
		fb.ProjectionMatrix = mySpotLightData->projection;
		fb.ViewMatrix = mySpotLightData->lightView;
		fb.FB_ScreenResolution = { shadowMap[0]->GetWidth(),shadowMap[0]->GetHeight() };
		fb.FB_CameraPosition = GetPosition();
		break;
	case uninitialized:
		break;
	default:
		break;
	}

	return fb;
}

void cLight::InspectorView()
{
	Component::InspectorView();
	Reflect<cLight>();
	using enum eLightType;
	switch (myLightType)
	{
	case Directional:
		Reflect<DirectionalLight>(*myDirectionLightData);
		break;
	case Point:
		Reflect<PointLight>(*myPointLightData);
		break;
	case Spot:
		Reflect<SpotLight>(*mySpotLightData);
		break;
	case uninitialized:
		break;
	}
}