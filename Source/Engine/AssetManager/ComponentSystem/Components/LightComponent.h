#pragma once 
#include "Engine/AssetManager/ComponentSystem/Component.h"

enum class eLightType
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
	friend class EnviromentLightPSO;
public:
	cLight() = delete; // Create a generic cube
	cLight(const unsigned int anOwnerId); // Create a generic cube 
	cLight(const unsigned int anOwnerId,const eLightType type);

	eLightType GetType() const;
	void SetType(const eLightType aType);

	void SetIsShadowCaster(bool active);
	bool GetIsShadowCaster();

	bool GetIsRendered() const;
	void SetIsRendered(bool isRendered);

	bool GetIsDirty() const;
	void SetIsDirty(bool dirty);

	void SetPower(float power);
	float GetPower();

	void SetColor(Vector3f color);
	Vector3f GetColor();

	void SetPosition(Vector3f position);
	Vector3f GetPosition();

	void SetDirection(Vector3f direction);
	Vector3f GetDirection();

	void SetRange(float range);
	float GetRange();

	void SetInnerAngle(float angle);
	float GetInnerAngle();

	void SetOuterAngle(float angle);
	float GetOuterAngle();

	std::shared_ptr<Texture> GetShadowMap(int number) const;
	void BindDirectionToTransform(bool active);
	bool GetIsBound();

	template<class T>
	std::shared_ptr<T> GetData();

	void Update() override;
	~cLight() = default;
private:
	void ConformToTransform();
	void RedrawShadowMap();
	void RedrawDirectionMap();
	void RedrawPointMap();
	void RedrawSpotMap();
	Matrix GetLightViewMatrix(int number);

	std::shared_ptr<DirectionalLight> myDirectionLightData;
	std::shared_ptr<SpotLight> mySpotLightData;
	std::shared_ptr<PointLight> myPointLightData;
	std::shared_ptr<Texture> shadowMap[6];

	eLightType myLightType;
	bool boundToTransform = false;
	bool isShadowCaster = true;
	bool isDirty = true;
	bool isRendered = false;
};

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