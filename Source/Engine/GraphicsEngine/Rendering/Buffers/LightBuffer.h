#pragma once 
#include <Engine/AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "../../Shaders/Registers.h"

struct LightBuffer
{
	DirectionalLight directionalLight = DirectionalLight();
	int pointLightAmount = 0;
	PointLight pointLight[8] = {};
	int spotLightAmount = 0;
	SpotLight spotLight[8] = {};
	float padding[2];
};