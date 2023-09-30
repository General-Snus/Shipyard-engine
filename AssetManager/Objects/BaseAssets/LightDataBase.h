#pragma once


struct SpotLight
{ 
	Vector3f Color = {1,1,1};
	float Power = 0;

	Vector3f Position = {0,0,0};	//12
	float Range = 0;		//4

	Vector3f Direction = {0,0,1}; //12
	float InnerConeAngle = 0; // 4

	float OuterConeAngle = 0; //4  
	Vector3f padding = {0,0,0};	//12
};

struct PointLight
{
	Vector3f Color;
	float Power;

	Vector3f Position;
	float Range;
};

struct DirectionalLight 
{
	Vector3f Color;
	float Power; // watt

	Vector3f Direction; 
	float padding;
};
