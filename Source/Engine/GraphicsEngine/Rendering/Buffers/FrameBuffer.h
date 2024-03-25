#pragma once
#include <array> 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vector4.hpp>

struct FrameBuffer
{
	Matrix ViewMatrix; // 64 bytes
	Matrix ProjectionMatrix; // 64 bytes
	float Time = 0; // 4 bytes 
	Vector3f FB_CameraPosition = { 0,0,0 };
	int FB_RenderMode = 0;
	Vector2i FB_ScreenResolution = { 1920,1080 };
	float padding = 0;
	//std::array<Vector4f,4> FB_FrustrumCorners = { Vector4f(),Vector4f(),Vector4f(),Vector4f() };
};