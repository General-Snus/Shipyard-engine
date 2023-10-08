#include "AssetManager.pch.h"
#include "LightComponent.h"
#include <Modelviewer/Core/Modelviewer.h>

cLight::cLight(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myLightType = eLightType::uninitialized;
	shadowMap = std::make_shared<Texture>();
	SetIsShadowCaster(true);
}
cLight::cLight(const unsigned int anOwnerId,const eLightType type) : Component(anOwnerId)
{
	myLightType = type;
	shadowMap = std::make_shared<Texture>();
	SetIsShadowCaster(true);

	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData = std::make_shared<DirectionalLight>();
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
void cLight::SetIsShadowCaster(bool active)
{
	isShadowCaster = active;

	if(active)
	{
		isDirty = true;
		std::wstring name = L"unNamedMap";
		CU::Vector2<int> resolution = {512,512};
		switch(myLightType)
		{
		case eLightType::Directional:
			name = L"directionalLight";
			resolution = {2048,2048};
			break;
		case eLightType::Point:
			name = L"pointLight";
			resolution = {512,512};
			break;
		case eLightType::Spot:
			name = L"spotLight";
			resolution = {512,512};
			break;
		case eLightType::uninitialized:
			break;
		default:
			break;
		} 

		if(!RHI::CreateTexture(shadowMap.get(),name,
			resolution.x,
			resolution.y,
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

bool cLight::GetIsShadowCaster()
{
	return false;
}
 

bool cLight::GetIsDirty() const
{
	return false;
}

void cLight::SetPower(float power)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData->Power = power;
		break;
	case eLightType::Point:
		myPointLightData->Power = power;
		break;
	case eLightType::Spot:
		mySpotLightData->Power = power;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
void cLight::SetColor(Vector3f color)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData->Color = color;
		break;
	case eLightType::Point:
		myPointLightData->Color = color;
		break;
	case eLightType::Spot:
		mySpotLightData->Color = color;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetColor()
{
	return Vector3f();
}
void cLight::SetPosition(Vector3f position)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		break;
	case eLightType::Point:
		myPointLightData->Position = position;
		break;
	case eLightType::Spot:
		mySpotLightData->Position = position;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetPosition()
{
	return Vector3f();
}
void cLight::SetDirection(Vector3f direction)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		myDirectionLightData->Direction = Vector4f(direction,1);
		RedrawDirectionMap(direction,*myDirectionLightData);
		break;
	case eLightType::Point:
		break;
	case eLightType::Spot:
		mySpotLightData->Direction = direction;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
Vector3f cLight::GetDirection()
{
	return Vector3f();
}
void cLight::SetRange(float range)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		break;
	case eLightType::Point:
		myPointLightData->Range = range;
		break;
	case eLightType::Spot:
		mySpotLightData->Range = range;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetRange()
{
	return 0.0f;
}
void cLight::SetInnerAngle(float angle)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		break;
	case eLightType::Point:
		break;
	case eLightType::Spot:
		mySpotLightData->InnerConeAngle = angle;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetInnerAngle()
{
	return 0.0f;
}
void cLight::SetOuterAngle(float angle)
{
	switch(myLightType)
	{
	case eLightType::Directional:
		break;
	case eLightType::Point:
		break;
	case eLightType::Spot:
		mySpotLightData->OuterConeAngle = angle;
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}

float cLight::GetOuterAngle()
{
	return 0.0f;
}

std::shared_ptr<Texture> cLight::GetShadowMap() const
{
	return shadowMap;
}

void cLight::Update()
{
	if(boundToTransform)
	{
		ConformToTransform();
	}
	if(isDirty)
	{

	}

}

void cLight::ConformToTransform()
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
		myDirectionLightData->Direction = Vector4f(transform->GetForward(),1);
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

void cLight::RedrawShadowMap()
{ 
	switch(myLightType)
	{
	case eLightType::Directional:
		RedrawDirectionMap(myDirectionLightData->Direction,*myDirectionLightData);
		break;
	case eLightType::Point: 
		break;
	case eLightType::Spot: 
		break;
	case eLightType::uninitialized:
		break;
	default:
		break;
	}
}
void cLight::RedrawDirectionMap()
{
	const float radius = ModelViewer::Get().GetWorldBounds().GetRadius();
	Vector3f lightPosition = radius * 2.0f * -myDirectionLightData->Direction.GetNormalized();
	const Vector3f worldCenter = ModelViewer::Get().GetWorldBounds().GetCenter();

	myDirectionLightData->Direction = Vector4f((worldCenter - lightPosition).GetNormalized(),1);
	myDirectionLightData->lightView = CU::Matrix4x4<float>::LookAt(lightPosition,worldCenter,{0,1,0}); // REFACTOR, Magic value up

	const Vector4f cameraCenter = Vector4f(worldCenter,0.0f) * myDirectionLightData->lightView;
	myDirectionLightData->lightView = Matrix::GetFastInverse(myDirectionLightData->lightView);

	const float leftPlane = cameraCenter.x - radius * 1;
	const float rightPlane = cameraCenter.x + radius * 1;
	const float bottomPlane = cameraCenter.y - radius * 1;
	const float topPlane = cameraCenter.y + radius * 1;
	const float nearPlane = 0.1f;
	const float farPlane = radius * 4;
	myDirectionLightData->projection = Matrix::CreateOrthographicProjection(
		leftPlane,
		rightPlane,
		bottomPlane,
		topPlane,
		nearPlane,
		farPlane);
}

void cLight::RedrawPointMap()
{

}

void cLight::RedrawSpotMap()
{
}

void cLight::BindDirectionToTransform(bool active)
{
	boundToTransform = active;
}