#include "AssetManager.pch.h"
#include "LightDataBase.h"

PointLight::PointLight() : LightDataBase()
{
	LightType = eLightType::Point;
	Range = 5000.0f;
	Intensity = 2000.0f;
} 

SpotLight::SpotLight() :  LightDataBase()
{
	LightType = eLightType::Spot;
	InnerConeAngle = 0.0f;
	OuterConeAngle = 0.0f;
	Range = 0.0f;
} 


DirectionalLight::DirectionalLight() : LightDataBase()
{
	LightType = eLightType::Directional;
	Direction = {0,-1,0};
} 

LightDataBase::LightDataBase() 
{
	LightType = eLightType::uninitialized;
	Color = Vector3f(1.0f, 1.0f, 1.0f);
	Intensity = 1.0f;
}

LightDataBase::~LightDataBase()
{
}
