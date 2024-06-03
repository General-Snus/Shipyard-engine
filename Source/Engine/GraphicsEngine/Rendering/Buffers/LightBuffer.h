#pragma once 
#include <Engine/AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "../../Shaders/Registers.h"

struct LightBuffer
{
	//Now we can retain default values in the light structs without splitting the limit.
	void NullifyMemory()
	{
		ZeroMemory(this,sizeof(LightBuffer));
	}
	int pointLightAmount = 0;
	int spotLightAmount = 0;
	float padding[2];

	DirectionalLight directionalLight = {};
	PointLight pointLight[8] = {};
	SpotLight spotLight[8] = {};
};