#include "AssetManager.pch.h"

#include "../LightComponent.h"
// #include <Editor/Editor/Core/Editor.h>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Math.hpp>

#include "DirectX/DX12/Graphics/GPU.h"
#include "DirectX/DX12/Graphics/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/LightDataBase.h"

Light::Light(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
	myLightType = eLightType::Directional;
}

Light::Light(const SY::UUID anOwnerId, GameObjectManager* aManager, const eLightType type)
	: Component(anOwnerId, aManager), myLightType(type)
{
}

void Light::Init()
{
	shadowMap[0] = std::make_shared<Texture>();
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		if (myDirectionLightData)
		{
			myDirectionLightData = std::make_shared<DirectionalLight>(*myDirectionLightData);
		}
		else
		{
			myDirectionLightData = std::make_shared<DirectionalLight>();
		}
		break;
	case Point:
		if (myPointLightData)
		{
			myPointLightData = std::make_shared<PointLight>(*myPointLightData);
		}
		else
		{
			myPointLightData = std::make_shared<PointLight>();
		}
		for (int i = 0; i < 6; i++)
		{
			shadowMap[i] = std::make_shared<Texture>();
		}
		break;
	case Spot:
		if (mySpotLightData)
		{
			mySpotLightData = std::make_shared<SpotLight>(*mySpotLightData);
		}
		else
		{
			mySpotLightData = std::make_shared<SpotLight>();
		}
		break;
	case uninitialized:
	default:
		LOGGER.Warn("Light was not initialized correctly, no/invalid type given");
		break;
	}
	SetIsShadowCaster(isShadowCaster);
}

eLightType Light::GetType() const
{
	return myLightType;
}

void Light::SetType(const eLightType aType)
{
	myLightType = aType;
}

bool Light::GetIsShadowCaster() const
{
	return isShadowCaster;
}

void Light::SetIsShadowCaster(const bool active)
{
	OPTICK_EVENT();
	isShadowCaster = active;

	if (active)
	{
		isDirty = true;
		std::wstring name = L"unNamedMap";
		Vector2ui    resolution = {512, 512};
		int          mapsToCreate = 0;
		switch (myLightType)
		{
			using enum eLightType;
		case Directional:
			name = L"directionalLight";
			resolution = {4096, 4096};
			mapsToCreate = 1;
			break;
		case Point:
			name = L"pointLight";
			resolution = {512, 512};
			mapsToCreate = 6;
			break;
		case Spot:
			name = L"spotLight";
			resolution = {1024, 1024};
			mapsToCreate = 1;
			break;
		case uninitialized:
			break;
		default:
			break;
		}

		for (int i = 0; i < mapsToCreate; i++)
		{
			if (!(!shadowMap[i] || shadowMap[i]->GetResolution() != resolution))
			{
				continue;
			}

			shadowMap[i] = std::make_shared<Texture>();

			shadowMap[i]->AllocateTexture(
				resolution,
				name + std::to_wstring(i) + L"_" + std::to_wstring(resolution.x) + L"|" + std::to_wstring(resolution.y),
				{}, DXGI_FORMAT_D32_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE);

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
	else
	{
		switch (myLightType)
		{
			using enum eLightType;
		case Directional:
			myDirectionLightData->castShadow = false;
			myDirectionLightData->shadowMapIndex = -1;
			break;
		case Point:
			myPointLightData->castShadow = false;
			for (size_t i = 0; i < 6; i++)
			{
				myPointLightData->shadowMapIndex[i] = -1;
			}
			break;
		case Spot:
			mySpotLightData->castShadow = false;
			mySpotLightData->shadowMapIndex = -1;
			break;
		case uninitialized:
			break;
		default:
			break;
		}

		for (const auto& tex : (shadowMap))
		{
			if (tex)
			{
				// shadowMap->reset();
				// TODO Make sure resource deletion is fine to do whenever we want
			}
		}
	}
}

bool Light::GetIsRendered() const
{
	return isRendered;
}

void Light::SetIsRendered(const bool shouldRender)
{
	this->isRendered = shouldRender;
}

bool Light::GetIsDirty() const
{
	return isDirty;
}

void Light::SetIsDirty(const bool dirty)
{
	isDirty = dirty;
	if (isShadowCaster)
	{
		SetIsRendered(false);
	}
}

void Light::SetPower(const float power)
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

float Light::GetPower() const
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

void Light::SetColor(const Color& color)
{
	m_Color = color;
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Color = m_Color.GetRGB();
		break;
	case Point:
		myPointLightData->Color = m_Color.GetRGB();
		break;
	case Spot:
		mySpotLightData->Color = m_Color.GetRGB();
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}

Color Light::GetColor() const
{
	return m_Color;
}

void Light::SetPosition(const Vector3f position)
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

Vector3f Light::GetPosition() const
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

void Light::SetDirection(const Vector3f direction)
{
	SetIsDirty(true);
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		myDirectionLightData->Direction = Vector4f(direction, 1);
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

Vector3f Light::GetDirection() const
{
	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		return Vector3f(myDirectionLightData->Direction.x, myDirectionLightData->Direction.y,
		                myDirectionLightData->Direction.z);
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

void Light::SetRange(const float range)
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

float Light::GetRange() const
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

void Light::SetInnerAngle(const float angle)
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

float Light::GetInnerAngle() const
{
	return 0.0f;
}

void Light::SetOuterAngle(const float angle)
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

float Light::GetOuterAngle() const
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

std::shared_ptr<Texture> Light::GetShadowMap(const int number) const
{
	return shadowMap[number];
}

void Light::Update()
{
	OPTICK_EVENT();
	if (boundToTransform)
	{
		ConformToTransform();
	}

	// TODO dealt with dynamic maps and static maps in some way
	// if (isDirty)
	{
		Vector3f color = m_Color.GetRGB();
		switch (myLightType)
		{
			using enum eLightType;
		case Directional:
			RedrawDirectionMap();
			myDirectionLightData->Color = color;
			myDirectionLightData->Power = std::max(0.0f, myDirectionLightData->Power);
			break;
		case Point:
			RedrawPointMap();
			myPointLightData->Color = color;
			myPointLightData->Power = std::max(0.0f, myPointLightData->Power);
			myPointLightData->Range = std::max(0.0f, myPointLightData->Range);
			break;
		case Spot:
			RedrawSpotMap();
			mySpotLightData->Color = color;
			mySpotLightData->InnerConeAngle =
				std::clamp(mySpotLightData->InnerConeAngle, 0.f, mySpotLightData->OuterConeAngle);
			mySpotLightData->OuterConeAngle =
				std::clamp(mySpotLightData->OuterConeAngle, mySpotLightData->InnerConeAngle, 360.f);
			mySpotLightData->Power = std::max(0.0f, mySpotLightData->Power);
			mySpotLightData->Range = std::max(0.0f, mySpotLightData->Range);
			break;
		case uninitialized:
			break;
		default:
			break;
		}
		SetIsDirty(false);
	}
}

void Light::ConformToTransform()
{
	OPTICK_EVENT();
	const Transform* transform = this->TryGetComponent<Transform>();
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
		myDirectionLightData->Direction = Vector4f(transform->GetForward(WORLD), 1);
		break;
	case Point:
		myPointLightData->Position = transform->GetPosition(WORLD);
		break;
	case Spot:
		mySpotLightData->Position = transform->GetPosition(WORLD);
		mySpotLightData->Direction = transform->GetForward(WORLD);
		break;
	case uninitialized:
		break;
	default:
		break;
	}
}

void Light::RedrawShadowMap()
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

void Light::RedrawDirectionMap()
{
	OPTICK_EVENT();
	constexpr float radius = 50;
	myDirectionLightData->Direction.Normalize();

	const Vector3f worldCenter = {};
	const Vector3f lightPosition = radius * 5.0f *
		-Vector3f(myDirectionLightData->Direction.x, myDirectionLightData->Direction.y,
		          myDirectionLightData->Direction.z);
	myDirectionLightData->lightView = Matrix::LookAt(lightPosition, worldCenter, {0, 1, 0}); // REFACTOR, Magic value up

	const Vector4f cameraCenter = Vector4f(worldCenter, 0.0f) * myDirectionLightData->lightView;
	myDirectionLightData->lightView = Matrix::GetFastInverse(myDirectionLightData->lightView);

	const float     leftPlane = cameraCenter.x - radius * 1;
	const float     rightPlane = cameraCenter.x + radius * 1;
	const float     bottomPlane = cameraCenter.y - radius * 1;
	const float     topPlane = cameraCenter.y + radius * 1;
	constexpr float nearPlane = 0.1f;
	constexpr float farPlane = radius * 5.f;
	myDirectionLightData->projection =
		Matrix::CreateOrthographicProjection(leftPlane, rightPlane, bottomPlane, topPlane, farPlane, nearPlane);
}

void Light::RedrawPointMap()
{
	OPTICK_EVENT();
	constexpr float fow = 90.0f * DEG_TO_RAD;
	constexpr float nearField = .01f;
	const float     farfield = std::max(myPointLightData->Range * 5, nearField + 0.0001f);

	const auto dxMatrix = XMMatrixPerspectiveFovLH(fow, 1, farfield, nearField);
	myPointLightData->projection = Matrix(&dxMatrix);

	for (int i = 0; i < 6; i++)
	{
		myPointLightData->lightView[i] = GetLightViewMatrix(i);
	}
}

void Light::RedrawSpotMap()
{
	OPTICK_EVENT();
	const Vector3f lightPosition = mySpotLightData->Position;
	mySpotLightData->lightView =
		Matrix::LookAt(lightPosition, lightPosition + mySpotLightData->Direction.GetNormalized(),
		               {0, 1, 0}); // REFACTOR, Magic value up

	const float     fow = mySpotLightData->OuterConeAngle;
	const float     farfield = mySpotLightData->Range * 2;
	constexpr float nearField = 0.01f;

	const auto dxMatrix = XMMatrixPerspectiveFovLH(fow * DEG_TO_RAD, 1, farfield, nearField);
	mySpotLightData->projection = Matrix(&dxMatrix);
	mySpotLightData->lightView = Matrix::GetFastInverse(mySpotLightData->lightView);
}

void Light::BindDirectionToTransform(const bool active)
{
	SetIsDirty(true);
	boundToTransform = active;
}

Matrix Light::GetLightViewMatrix(const int number) const
{
	const Vector3f lightPos = myPointLightData->Position;
	assert(number < 6 && "There are only 6 faces to a cubemap");
	assert(myLightType == eLightType::Point && "Use only for point lights");
	switch (number)
	{
	case 0:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)));
		break;
	case 1:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)));
		break;
	case 2:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f)));
		break;
	case 3:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f)));
		break;
	case 4:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f)));
		break;
	case 5:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f)));
		break;
	default:
		return Matrix::GetFastInverse(
			Matrix::LookAt(lightPos, lightPos + Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)));
		break;
	}
}

bool Light::GetIsBound() const
{
	return boundToTransform;
}

FrameBuffer Light::GetShadowMapFrameBuffer(const int number) const
{
	FrameBuffer fb;

	switch (myLightType)
	{
		using enum eLightType;
	case Directional:
		fb.ProjectionMatrix = myDirectionLightData->projection;
		fb.ViewMatrix = myDirectionLightData->lightView;
		fb.FB_ScreenResolution = {shadowMap[0]->GetWidth(), shadowMap[0]->GetHeight()};
		fb.FB_CameraPosition = GetPosition();
		break;
	case Point:
		fb.ProjectionMatrix = myPointLightData->projection;
		fb.ViewMatrix = GetLightViewMatrix(number);
		fb.FB_ScreenResolution = {shadowMap[number]->GetWidth(), shadowMap[number]->GetHeight()};
		fb.FB_CameraPosition = GetPosition();
		break;
	case Spot:
		fb.ProjectionMatrix = mySpotLightData->projection;
		fb.ViewMatrix = mySpotLightData->lightView;
		fb.FB_ScreenResolution = {shadowMap[0]->GetWidth(), shadowMap[0]->GetHeight()};
		fb.FB_CameraPosition = GetPosition();
		break;
	case uninitialized:
		break;
	default:
		break;
	}

	return fb;
}

bool Light::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}

	for (const auto& [index, tex] : std::ranges::views::enumerate(shadowMap))
	{
		if (tex)
		{
			ImGui::Text("ShadowMap_%i:  %s", index, tex->GetName().c_str());
			ImGui::Text("ShadowMap_%i:  %i", index, tex->GetHeapOffset());
		}
	}

	const bool previousShadowCasterValue = isShadowCaster;
	Reflect<Light>();

	// stupid stupid stupid, no textures would be allocated otherwise
	if (isShadowCaster != previousShadowCasterValue)
	{
		SetIsShadowCaster(isShadowCaster);
	}

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
	return true;
}
