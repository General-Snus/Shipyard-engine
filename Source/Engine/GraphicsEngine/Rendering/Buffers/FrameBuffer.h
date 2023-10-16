#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/ThirdParty/CU/CommonUtills/Matrix4x4.hpp>

struct FrameBufferData
{
	CommonUtilities::Matrix4x4<float> ViewMatrix; // 64 bytes
	CommonUtilities::Matrix4x4<float> ProjectionMatrix; // 64 bytes
	float Time = 0; // 4 bytes
	float FB_CameraPosition[3] = {};// 12 bytes padding  

	int FB_RenderMode;
	int FB_ScreenResolution[2];
	float padding;

};

typedef ConstantBuffer<FrameBufferData> FrameBuffer;
