#pragma once  
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"

struct SpotLight
{
	Vector3f Color = { 1,1,1 };
	float Power = 0;

	Vector3f Position = { 0,0,0 };	//12
	float Range = 0;		//4

	Vector3f Direction = { 0,0,1 }; //12
	float InnerConeAngle = 0; // 4

	float OuterConeAngle = 0; //4  
	int shadowMapIndex;
	Vector2f padding = { 0,0 };	//12

	Matrix lightView;
	Matrix projection;
};

struct PointLight
{
	Vector3f Color = { 1,1,1 };
	float Power = 0;

	Vector3f Position = { 0,0,0 };
	float Range = 0;

	Matrix lightView;
	Matrix projection;


	int shadowMapIndex[6] = { -1 };
	float padding[2]{};
};

struct DirectionalLight
{
	Vector3f Color;
	float Power = 5; // watt

	Vector4f Direction = { 0,0,1,1 };

	Matrix lightView;
	Matrix projection;

	int shadowMapIndex;
	float padding[3];
};