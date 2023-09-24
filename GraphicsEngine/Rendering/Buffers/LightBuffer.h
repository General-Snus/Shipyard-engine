#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 
#include <AssetManager/Objects/BaseAssets/LightDataBase.h>

struct LightBufferData 
{
   DirectionalLight myDirectionalLight;
   PointLight myPointLight[8];
   SpotLight mySpotLight[8]; 
};

typedef ConstantBuffer<LightBufferData> LightBuffer;