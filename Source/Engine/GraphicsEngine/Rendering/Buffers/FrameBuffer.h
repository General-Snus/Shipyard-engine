#pragma once
#include <array>
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

struct FrameBufferData
{
	Matrix ViewMatrix; // 64 bytes
	Matrix ProjectionMatrix; // 64 bytes
	float Time = 0; // 4 bytes 
	std::array<float,3> FB_CameraPosition = { 0,0,0 };
	int FB_RenderMode = 0;
	std::array<int,2> FB_ScreenResolution = { 1920,1080 };
	float padding = 0;
	std::array<Vector4f,4> FB_FrustrumCorners = { Vector4f(),Vector4f(),Vector4f(),Vector4f() };

};

using FrameBuffer = ConstantBuffer<FrameBufferData>;
