#pragma once 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

struct ObjectBuffer
{
	Matrix myTransform{};
	Vector3f MinExtents = -Vector3f(1,1,1);;
	alignas(4) bool hasBone = false;
	Vector3f MaxExtents = Vector3f(1,1,1);;
	uint32_t uniqueID{};
	//std::array < Matrix,128> myBoneTransforms; // 64 * 128 = 8192 bytes
};
