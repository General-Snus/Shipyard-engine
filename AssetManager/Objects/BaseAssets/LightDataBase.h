#pragma once


struct SpotLight
{ 
	Vector3f Color;
	float Intensity;

	Vector3f Position;	//12
	float Range;		//4

	Vector3f Direction; //12
	float InnerConeAngle; // 4

	float OuterConeAngle; //4  
	Vector3f padding;	//12
};

struct PointLight
{
	Vector3f Color;
	float Intensity;

	Vector3f Position;
	float Range;
};

struct DirectionalLight 
{
	Vector3f Color;
	float Intensity;

	Vector3f Direction; 
	float padding;
};
