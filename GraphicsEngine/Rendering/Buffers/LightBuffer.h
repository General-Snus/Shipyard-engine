#pragma once
#include <GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>
#include <AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "../../Shaders/Registers.h"

struct LightBufferData 
{
   DirectionalLight myDirectionalLight= DirectionalLight();
   PointLight myPointLight  = {PointLight()};
   SpotLight mySpotLight = {SpotLight()};;
};

typedef ConstantBuffer<LightBufferData> LightBuffer;