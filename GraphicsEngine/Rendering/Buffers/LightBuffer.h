#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 
#include <AssetManager/Objects/BaseAssets/LightDataBase.h>
#include "../../Shaders/Registers.h"

struct LightBufferData 
{
   DirectionalLight myDirectionalLight= DirectionalLight();
   PointLight myPointLight[PointLightCount] = {PointLight()};
   SpotLight mySpotLight[SpotLightCount] = {SpotLight()};;
};

typedef ConstantBuffer<LightBufferData> LightBuffer;