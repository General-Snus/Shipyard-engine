#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct ObjectBufferData
{
	CU::Matrix4x4<float> myTransform; // 64 bytes
	CU::Vector3<float> MinExtents; // 12 bytes
	float padding1 = 0; // 4 bytes 
	CU::Vector3<float> MaxExtents; // 12 bytes
	alignas(4) bool hasBone = false;
	std::array <CU::Matrix4x4<float>,128> myBoneTransforms; // 64 * 128 = 8192 bytes
};

using ObjectBuffer = ConstantBuffer<ObjectBufferData> ;
