#pragma once 
#include <Engine/AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "cstring"

struct LightBuffer
{
	//Now we can retain default values in the light structs without splitting the limit.
	void NullifyMemory()
	{
		memset(this,0,sizeof(LightBuffer));
	}
	int pointLightAmount = 0;
	int spotLightAmount = 0;
	std::array<float,2> padding;

	DirectionalLight directionalLight = {};
	std::array<PointLight,8> pointLight = {};
	std::array<SpotLight,8> spotLight{};
};
