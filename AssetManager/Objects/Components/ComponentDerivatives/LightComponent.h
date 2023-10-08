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
	void GetType(const eLightType aType);

	void SetIsShadowCaster(bool active);
	bool GetIsShadowCaster();

	bool GetIsDirty() const;

	void SetPower(float power);
	float GetPower(float power);

	void SetColor(Vector3f color);
	Vector3f GetColor();

	void SetPosition(Vector3f position);
	Vector3f GetPosition( );

	void SetDirection(Vector3f direction);
	Vector3f GetDirection( );

	void SetRange(float range);
	float GetRange( );

	void SetInnerAngle(float angle);
	float GetInnerAngle( );

	void SetOuterAngle(float angle);
	float GetOuterAngle( ); 

	std::shared_ptr<Texture> GetShadowMap() const;
	void BindDirectionToTransform(bool active);

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

	std::shared_ptr<DirectionalLight> myDirectionLightData;
	std::shared_ptr<SpotLight> mySpotLightData;
	std::shared_ptr<PointLight> myPointLightData;
	std::shared_ptr<Texture> shadowMap;

	eLightType myLightType;
	bool boundToTransform = false;
	bool isShadowCaster = true; 
	bool isDirty = true; 
};

template<>
inline std::shared_ptr<DirectionalLight> cLight::GetData<DirectionalLight>()
{ 
	RedrawDirectionMap(); // make sure is fresh with this hard call
	return myDirectionLightData;
}

template<>
inline std::shared_ptr<SpotLight> cLight::GetData<SpotLight>()
{
	RedrawSpotMap(); // make sure is fresh with this hard call
	return mySpotLightData;
}

template<>
inline std::shared_ptr<PointLight> cLight::GetData<PointLight>()
{
	RedrawPointMap(); // make sure is fresh with this hard call
	return myPointLightData;
} 