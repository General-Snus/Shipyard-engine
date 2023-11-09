#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct LineBufferData
{
     Matrix myTransform; // 64 bytes 
};

typedef ConstantBuffer<LineBufferData> LineBuffer;
