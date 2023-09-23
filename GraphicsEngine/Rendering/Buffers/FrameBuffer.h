#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 

struct FrameBufferData {
	CommonUtilities::Matrix4x4<float> ViewMatrix; // 64 bytes
	CommonUtilities::Matrix4x4<float> ProjectionMatrix; // 64 bytes
	float Time = 0; // 4 bytes
	float FB_CameraPosition[3] = {};// 12 bytes padding  


};

typedef ConstantBuffer<FrameBufferData> FrameBuffer;
