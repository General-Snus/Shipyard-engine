#pragma once 
#include <Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include "Engine/AssetManager/ComponentSystem/Component.h"

enum class eLightType : unsigned int
{
	Directional = 0,
	Point = 1,
	Spot = 2,
	uninitialized = 3
};
struct DirectionalLight;
struct SpotLight;
class Texture;
struct PointLight;
class cLight : public Component
{


	friend class GraphicsEngine;
	friend class ShadowRenderer;
	friend class EnvironmentLightPSO;
public:
	MYLIB_REFLECTABLE();
	cLight() = delete; // Create a generic cube
	cLight(const SY::UUID anOwnerId,GameObjectManager* aManager); // Create a generic cube 
	cLight(const SY::UUID anOwnerId,GameObjectManager* aManager,const eLightType type);

	eLightType GetType() const;
	void SetType(const eLightType aType);

	bool GetIsShadowCaster() const;
	void SetIsShadowCaster(bool active);

	bool GetIsRendered() const;
	void SetIsRendered(bool isRendered);

	bool GetIsDirty() const;
	void SetIsDirty(bool dirty);

	float GetPower() const;
	void SetPower(float power);

	Vector3f GetColor() const;
	void SetColor(Vector3f color);

	void SetPosition(Vector3f position);
	Vector3f GetPosition() const;

	void SetDirection(Vector3f direction);
	Vector3f GetDirection() const;

	void SetRange(float range);
	float GetRange() const;

	void SetInnerAngle(float angle);
	float GetInnerAngle() const;

	void SetOuterAngle(float angle);
	float GetOuterAngle() const;

	std::shared_ptr<Texture> GetShadowMap(int number) const;
	void BindDirectionToTransform(bool active);
	bool GetIsBound() const;

	FrameBuffer GetShadowMapFrameBuffer(const int number = 0) const;
	bool InspectorView() override;

	template<class T>
	std::shared_ptr<T> GetData();

	void Update() override;
	~cLight() override = default;

	bool boundToTransform = true;
	bool isShadowCaster = true;
	bool isDirty = true;
	bool isRendered = false;

private:
	void ConformToTransform();
	void RedrawShadowMap();
	void RedrawDirectionMap();
	void RedrawPointMap();
	void RedrawSpotMap();
	Matrix GetLightViewMatrix(int number) const;


	eLightType myLightType;
	std::shared_ptr<DirectionalLight> myDirectionLightData;
	std::shared_ptr<SpotLight> mySpotLightData;
	std::shared_ptr<PointLight> myPointLightData;
	std::shared_ptr<Texture> shadowMap[6];

};

REFL_AUTO(
	type(cLight),
	field(isRendered),
	field(isDirty),
	field(isShadowCaster),
	field(boundToTransform)
)


template<>
inline std::shared_ptr<DirectionalLight> cLight::GetData<DirectionalLight>()
{
	return myDirectionLightData;
}

template<>
inline std::shared_ptr<SpotLight> cLight::GetData<SpotLight>()
{
	return mySpotLightData;
}

template<>
inline std::shared_ptr<PointLight> cLight::GetData<PointLight>()
{
	return myPointLightData;
}