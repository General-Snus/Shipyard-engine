#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct ObjectBufferData
{
	Matrix myTransform; // 64 bytes
	Vector3f MinExtents; // 12 bytes
	alignas(4) bool hasBone = false; //4
	Vector3f MaxExtents; // 12 bytes
	alignas(4) bool isInstanced = false;//4
	std::array < Matrix,128> myBoneTransforms; // 64 * 128 = 8192 bytes
};

using ObjectBuffer = ConstantBuffer<ObjectBufferData>;
