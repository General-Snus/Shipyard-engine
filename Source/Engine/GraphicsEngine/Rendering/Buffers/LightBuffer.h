#pragma once 
#include <Engine/AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "../../Shaders/Registers.h"

struct LightBuffer
{
	int pointLightAmount = 0;
	int spotLightAmount = 0;
	float padding[2];

	DirectionalLight directionalLight = DirectionalLight();
	PointLight pointLight[8] = {};
	SpotLight spotLight[8] = {};
};