#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 

struct LineBufferData
{
    CU::Matrix4x4<float> myTransform; // 64 bytes 
};

typedef ConstantBuffer<LineBufferData> LineBuffer;
