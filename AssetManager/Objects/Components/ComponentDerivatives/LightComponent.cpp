#include "AssetManager.pch.h"
#include "LightComponent.h"
#include <Modelviewer/Core/Modelviewer.h>

cLight::cLight(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myLightType = eLightType::uninitialized;
	shadowMap = std::make_shared<Texture>();
	if(castShadows)
	{
		if(!RHI::CreateTexture(shadowMap.get(),L"directionalLight",
			2048,
			2048,
			DXGI_FORMAT_R32_TYPELESS,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
		))
		{
			std::cout << "Error in texture creation shadowmap" << std::endl;
		}
		RHI::ClearDepthStencil(shadowMap.get());
	}
}
cLight::cLight(const unsigned int anOwnerId,const eLightType type) : Component(anOwnerId)
{
	myLightType = type;
	shadowMap = std::make_shared<Texture>();
	if(castShadows)
	{
		if(!RHI::CreateTexture(shadowMap.get(),L"directionalLight",
			2048,
			2048,
			DXGI_FORMAT_R32_TYPELESS,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
		))
		{
			std::cout << "Error in texture creation shadowmap" << std::endl;
		}
		RHI::ClearDepthStencil(shadowMap.get());
	}


	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData = std::make_shared<DirectionalLight>();
		CalculateDirectionLight({0.0f,-1.0f,0.0f},*myDirectionLightData);
		break;
	case eLightType::Point:
		myPointLightData = std::make_shared<PointLight>();
		break;
	case eLightType::Spot:
		mySpotLightData = std::make_shared<SpotLight>();
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
eLightType cLight::GetType() const
{
	return myLightType;
}
void cLight::SetType(const eLightType aType)
{
	myLightType = aType;
}

std::shared_ptr<Texture> cLight::GetShadowMap() const
{
	return shadowMap;
}

void cLight::Update()
{
	if(boundToTransform)
	{
		Transform* transform = this->TryGetComponent<Transform>();

		if(transform == nullptr)
		{
			std::cout << "Light component has no transform component" << std::endl;
			return;
		}
		switch(myLightType)
		{
		case eLightType::Directional:
			CalculateDirectionLight(transform->GetForward(),*myDirectionLightData);
			break;
		case eLightType::Point:
			myPointLightData->Position = transform->GetPosition();
			break;
		case eLightType::Spot:
			mySpotLightData->Position = transform->GetPosition();
			mySpotLightData->Direction = transform->GetForward();
			break;
		case eLightType::uninitialized:
			break;
		default:
			break;
		}
	}
}
void cLight::CalculateDirectionLight(Vector3f direction, DirectionalLight& ref)
{ 
	const float radius = ModelViewer::Get().GetWorldBounds().GetRadius();
	Vector3f lightPosition = radius * 2.0f * -direction.GetNormalized();
	const Vector3f worldCenter = ModelViewer::Get().GetWorldBounds().GetCenter();
	ref.Direction =  Vector4f((worldCenter - lightPosition).GetNormalized(),1);
	ref.lightView = CU::Matrix4x4<float>::LookAt(lightPosition,worldCenter,{0,1,0}); // REFACTOR, Magic value up
	const Vector4f cameraCenter = Vector4f(worldCenter,0.0f) * ref.lightView; 

	const float leftPlane = cameraCenter.x - radius * 1;
	const float rightPlane = cameraCenter.x + radius * 1;
	const float bottomPlane = cameraCenter.y - radius * 1;
	const float topPlane = cameraCenter.y + radius * 1;
	const float nearPlane = 0.1f;
	const float farPlane = radius * 10;
	ref.projection = Matrix::CreateOrthographicProjection(
		leftPlane,
		rightPlane,
		bottomPlane,
		topPlane,
		nearPlane,
		farPlane); 
}
void cLight::BindDirectionToTransform(bool active)
{
	boundToTransform = active;
}