#pragma once 
#include <AssetManager/AssetManager.pch.h>
#include <AssetManager/AssetManager.h>
#include "../../BaseAssets/LightDataBase.h"

enum class eLightType
{
	Directional = 0,
	Point = 1,
	Spot = 2,
	uninitialized = 3
};

class cLight : public Component
{
public:
	cLight() = delete; // Create a generic cube
	cLight(const unsigned int anOwnerId); // Create a generic cube 
	cLight(const unsigned int anOwnerId,const eLightType type);

	eLightType GetType() const;
	void SetType(const eLightType aType);
	void BindDirectionToTransform(bool active);

	template<class T>
	std::shared_ptr<T> GetData();
	std::shared_ptr<Texture> GetShadowMap() const;
	void Update() override; 

	~cLight() = default;
private:
	eLightType myLightType;
	std::shared_ptr<DirectionalLight> myDirectionLightData;
	std::shared_ptr<SpotLight> mySpotLightData;
	std::shared_ptr<PointLight> myPointLightData;
	std::shared_ptr<Texture> shadowMap;
	bool boundToTransform = false;
	bool castShadows = true;
};

template<>
inline std::shared_ptr<DirectionalLight> cLight::GetData<DirectionalLight>()
{
	myDirectionLightData->CalculateDirectionLight(myDirectionLightData->Direction);
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