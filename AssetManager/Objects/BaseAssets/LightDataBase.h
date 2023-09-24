#pragma once

enum class eLightType
{
	Directional = 0,
	Point = 1,
	Spot = 2,
	uninitialized = 3
};
class LightDataBase
{
public:
	LightDataBase();
	~LightDataBase() ;  
	Vector3f Color;
	float Intensity;
	eLightType LightType;
};

class SpotLight : public LightDataBase
{
public:
	SpotLight();
	Vector3f Position;
	Vector3f Direction;
	float Range;
	float InnerConeAngle;
	float OuterConeAngle;
};

class PointLight : public LightDataBase
{
public:
	PointLight();
	Vector3f Position;
	float Range;
};

class DirectionalLight : public LightDataBase
{
public:
	DirectionalLight();
	Vector3f Direction; 
};
