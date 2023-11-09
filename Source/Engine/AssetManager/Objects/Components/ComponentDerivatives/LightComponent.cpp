#include "LightComponent.h"
#include "AssetManager.pch.h"
//#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/Math.hpp>
#include "../../BaseAssets/LightDataBase.h"

cLight::cLight(const unsigned int anOwnerId) : Component(anOwnerId),isDirty(true)
{
	myLightType = eLightType::uninitialized;
	shadowMap[0] = std::make_shared<Texture>();
	SetIsShadowCaster(true);
}

cLight::cLight(const unsigned int anOwnerId,const eLightType type) : Component(anOwnerId)
{
	myLightType = type;
	shadowMap[0] = std::make_shared<Texture>();

	switch(myLightType)
	{
	using enum eLightType;
	case Directional:
		myDirectionLightData = std::make_shared<DirectionalLight>();
		break;
	case Point:
		myPointLightData = std::make_shared<PointLight>();
		for(int i = 0; i < 6; i++)
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

bool cLight::GetIsShadowCaster()
{
	return isShadowCaster;
}
void cLight::SetIsShadowCaster(bool active)
{
	isShadowCaster = active;

	if(active)
	{
		isDirty = true;
		std::wstring name = L"unNamedMap";
		CU::Vector2 resolution = {512,512};
		int mapsToCreate = 0;
		switch(myLightType)
		{
		using enum eLightType;
		case Directional:
			name = L"directionalLight";
			resolution = {4096,4096};
			mapsToCreate = 1;
			break;
		case Point:
			name = L"pointLight";
			resolution = {512,512};
			mapsToCreate = 6;
			break;
		case Spot:
			name = L"spotLight";
			resolution = {1024,1024};
			mapsToCreate = 1;
			break;
		case uninitialized:
			break;
		default:
			break;
		}

		for(int i = 0; i < mapsToCreate; i++)
		{
			shadowMap[i] = std::make_shared<Texture>(); 

			if(!RHI::CreateTexture(
				shadowMap[i].get(),
				name
				+ std::to_wstring(i) + L"_"
				+ std::to_wstring(resolution.x) + L"|"
				+ std::to_wstring(resolution.y),
				resolution.x,
				resolution.y,
				DXGI_FORMAT_R32_TYPELESS,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
			))
			{
				std::cout << "Error in texture creation shadowmap" << std::endl;
			}

			RHI::ClearDepthStencil(shadowMap[i].get());
		}
	}
}

bool cLight::GetIsRendered() const
{
	return isRendered;
}
void cLight::SetIsRendered(bool aRendered)
{
	isRendered = aRendered;
}

bool cLight::GetIsDirty() const
{
	return isDirty;
}
void cLight::SetIsDirty(bool dirty)
{
	isDirty = dirty;
	if(isShadowCaster)
	{
		SetIsRendered(false);
	}
}

void cLight::SetPower(float power)
{
	SetIsDirty(true);
	switch(myLightType)
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
float cLight::GetPower()
{
	switch(myLightType)
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

void cLight::SetColor(Vector3f color)
{
	SetIsDirty(true);
	switch(myLightType)
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
Vector3f cLight::GetColor()
{
	switch(myLightType)
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

void cLight::SetPosition(Vector3f position)
{
	SetIsDirty(true);
	switch(myLightType)
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
Vector3f cLight::GetPosition()
{
	switch(myLightType)
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

void cLight::SetDirection(Vector3f direction)
{
	SetIsDirty(true);
	switch(myLightType)
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
Vector3f cLight::GetDirection()
{
	switch(myLightType)
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

void cLight::SetRange(float range)
{
	SetIsDirty(true);
	switch(myLightType)
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
float cLight::GetRange()
{
	switch(myLightType)
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

void cLight::SetInnerAngle(float angle)
{

	SetIsDirty(true);
	switch(myLightType)
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
float cLight::GetInnerAngle()
{
	return 0.0f;
}

void cLight::SetOuterAngle(float angle)
{

	SetIsDirty(true);
	switch(myLightType)
	{
	using enum eLightType;
	case Directional:
		break;
	case Point:
		break;
	case Spot:
		mySpotLightData->OuterConeAngle = angle * DEG_TO_RAD;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}
float cLight::GetOuterAngle()
{
	switch(myLightType)
	{
	using enum eLightType;
	case Directional:
		return 0.0f;
		break;
	case Point:
		return 0.0f;
		break;
	case Spot:
		return mySpotLightData->OuterConeAngle * RAD_TO_DEG;
		break;
	case uninitialized:
		break;
	default:
		break;
	}
	return 0.0f;
}

std::shared_ptr<Texture> cLight::GetShadowMap(int number) const
{
	return shadowMap[number];
}

void cLight::Update()
{
	if(boundToTransform)
	{
		ConformToTransform();
	}
	if(isDirty)
	{
		switch(myLightType)
		{
		using enum eLightType;
		case Directional:
			RedrawDirectionMap();
			break;
		case Point:
			RedrawPointMap();
			break;
		case Spot:
			RedrawSpotMap();
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
	Transform* transform = this->TryGetComponent<Transform>();
	if(transform == nullptr)
	{
		std::cout << "Light component has no transform component" << std::endl;
		return;
	}
	if(transform->GetIsDirty())
	{
		SetIsDirty(true);
	}
	switch(myLightType)
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
	switch(myLightType)
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
	//TODO GET ACTIVE SCENE
	const float radius = 100000;//ModelViewer::Get().GetWorldBounds().GetRadius();
	 Vector3f lightPosition = radius * 2.0f * -Vector3f(myDirectionLightData->Direction.GetNormalized().x,myDirectionLightData->Direction.GetNormalized().y,myDirectionLightData->Direction.GetNormalized().z);
	 const Vector3f worldCenter = Vector3f();// ModelViewer::Get().GetWorldBounds().GetCenter();

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
	Vector3f lightPosition = myPointLightData->Position;
	myPointLightData->lightView = CU::Matrix4x4<float>::LookAt(lightPosition,lightPosition + GlobalFwd,{0,1,0}); // REFACTOR, Magic value up

	const float fow = 90.0f * DEG_TO_RAD;
	const float fowmdf = 1.0f / (tanf(fow / 2.0f));
	const float farfield = myPointLightData->Range * 5;
	const float nearField = 1.0f;
	const float prc = farfield / (farfield - nearField);
	Matrix clipMatrix;
	clipMatrix(1,1) = fowmdf;
	clipMatrix(2,2) = fowmdf;
	clipMatrix(3,3) = prc;
	clipMatrix(3,4) = 1 / prc;
	clipMatrix(4,3) = nearField * -prc;
	clipMatrix(4,4) = 0;

	myPointLightData->projection = clipMatrix;
	myPointLightData->lightView = Matrix::GetFastInverse(myPointLightData->lightView);
}

void cLight::RedrawSpotMap()
{
	Vector3f lightPosition = mySpotLightData->Position;
	mySpotLightData->lightView = CU::Matrix4x4<float>::LookAt(lightPosition,lightPosition + mySpotLightData->Direction.GetNormalized(),{0,1,0}); // REFACTOR, Magic value up

	const float fow = mySpotLightData->OuterConeAngle;
	const float fowmdf = 1.0f / (tanf(fow / 2.0f));
	const float farfield = mySpotLightData->Range * 2;
	const float nearField = 1.0f;
	const float prc = farfield / (farfield - nearField);
	Matrix clipMatrix;
	clipMatrix(1,1) = fowmdf;
	clipMatrix(2,2) = fowmdf;
	clipMatrix(3,3) = prc;
	clipMatrix(3,4) = 1 / prc;
	clipMatrix(4,3) = nearField * -prc;
	clipMatrix(4,4) = 0;

	mySpotLightData->projection = clipMatrix;
	mySpotLightData->lightView = Matrix::GetFastInverse(mySpotLightData->lightView);
}

void cLight::BindDirectionToTransform(bool active)
{
	SetIsDirty(true);
	boundToTransform = active;
}

Matrix cLight::GetLightViewMatrix(int number)
{
	Vector3f lightPos = myPointLightData->Position;
	assert(number < 6 && "There are only 6 faces to a cubemap");
	assert(myLightType == eLightType::Point && "Use only for point lights");
	switch(number)
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

bool cLight::GetIsBound()
{
	return boundToTransform;
}
