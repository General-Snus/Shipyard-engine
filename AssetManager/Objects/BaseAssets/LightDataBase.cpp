#include "AssetManager.pch.h"
#include "LightDataBase.h"
#include <ThirdParty/CU/Math.hpp>

PointLight::PointLight() : LightDataBase()
{
	LightType = eLightType::Point;
	Range = 500.0f;
	Intensity = 0.0f;
} 

SpotLight::SpotLight() :  LightDataBase()
{
	LightType = eLightType::Spot;
	Direction = {0,-1,0};
	InnerConeAngle = 10.0f * DEG_TO_RAD;
	OuterConeAngle = 45.0f * DEG_TO_RAD;
	Range = 500.0f;
	Intensity = 0.0f;
} 


DirectionalLight::DirectionalLight() : LightDataBase()
{
	LightType = eLightType::Directional;
	Direction = {0,-1,0};
	Intensity = 0.0f;
} 

LightDataBase::LightDataBase() 
{
	LightType = eLightType::uninitialized;
	Color = Vector3f(1.0f, 1.0f, 1.0f);
	Intensity = 0.0f;
}

LightDataBase::~LightDataBase()
{
} 
