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
	~LightDataBase();
	Vector3f Color;			//12
	float Intensity;		//4
	eLightType LightType;	//4
private:
	Vector3f padding;			//12
};

class SpotLight : public LightDataBase
{
public:
	SpotLight();
	Vector3f Position;	//12
	float Range;		//4

	Vector3f Direction; //12
	float InnerConeAngle; // 4

	float OuterConeAngle; //4 
private:
	Vector3f padding;			//12
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
private:
	float padding;
};
