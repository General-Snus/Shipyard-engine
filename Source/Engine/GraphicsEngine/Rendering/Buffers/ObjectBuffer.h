#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct ObjectBufferData {
    CU::Matrix4x4<float> myTransform; // 64 bytes
    CU::Vector3<float> MinExtents; // 12 bytes
    float padding1; // 4 bytes 
    CU::Vector3<float> MaxExtents; // 12 bytes
    alignas(4) bool hasBone; //Should be 1 but is prbl 4 bytes
    CU::Matrix4x4<float> myBoneTransforms[128]; // 64 * 128 = 8192 bytes
};

typedef ConstantBuffer<ObjectBufferData> ObjectBuffer;
