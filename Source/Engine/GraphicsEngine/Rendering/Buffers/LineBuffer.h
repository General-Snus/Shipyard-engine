#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct LineBufferData
{
    CU::Matrix4x4<float> myTransform; // 64 bytes 
};

typedef ConstantBuffer<LineBufferData> LineBuffer;
