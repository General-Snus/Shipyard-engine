#pragma once 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

struct ObjectBuffer
{
	Matrix myTransform;
	Vector3f MinExtents;
	alignas(4) bool hasBone = false;
	Vector3f MaxExtents;
	alignas(4) bool isInstanced = false;
	//std::array < Matrix,128> myBoneTransforms; // 64 * 128 = 8192 bytes
};
