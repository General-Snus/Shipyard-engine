#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 

struct LightBufferData
{
    CommonUtilities::Vector4<float> lightDirection;
    CommonUtilities::Vector4<float> lightPosition;
    CommonUtilities::Vector4<float> lightDiffuseColor;
    CommonUtilities::Vector4<float> lightSpecularColor;

};

typedef ConstantBuffer<LightBufferData> ObjectBuffer;