#include "AssetManager.pch.h"
#include "LightComponent.h"

cLight::cLight(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myLightType = eLightType::uninitialized; 
	shadowMap = std::make_shared<Texture>();
	if(castShadows)
	{
		RHI::CreateTexture(shadowMap.get(),L"directionalLight",
			1024,
			1024,
			DXGI_FORMAT_R32_TYPELESS,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
		);
		RHI::ClearDepthStencil(shadowMap.get());
	}
}
cLight::cLight(const unsigned int anOwnerId,const eLightType type) : Component(anOwnerId)
{
	myLightType = type;
	shadowMap = std::make_shared<Texture>();
	if(castShadows)
	{
		RHI::CreateTexture(shadowMap.get(),L"directionalLight",
			1024,
			1024,
			DXGI_FORMAT_R32_TYPELESS,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
		);
		RHI::ClearDepthStencil(shadowMap.get());
	}


	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData = std::make_shared<DirectionalLight>();
		myDirectionLightData->CalculateDirectionLight({0.0f,-1.0f,0.0f});
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
			myDirectionLightData->CalculateDirectionLight(transform->GetForward());  
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
void cLight::BindDirectionToTransform(bool active)
{
	boundToTransform = active;
}