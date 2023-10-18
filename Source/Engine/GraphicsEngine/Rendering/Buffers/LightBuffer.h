#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h> 
#include <Engine/AssetManager/Objects/BaseAssets/LightDataBase.h> 
#include "../../Shaders/Registers.h"

struct LightBufferData 
{
   DirectionalLight myDirectionalLight= DirectionalLight();
   PointLight myPointLight  = {PointLight()};
   SpotLight mySpotLight = {SpotLight()};;
};

typedef ConstantBuffer<LightBufferData> LightBuffer;