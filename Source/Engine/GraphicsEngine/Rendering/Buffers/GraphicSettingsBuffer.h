#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>  
#include "../../Shaders/Registers.h"

struct GraphicSettingsBufferData
{
	int GSB_ToneMap; // 4 
	float GSB_AO_intensity = 0.35f;
	float GSB_AO_scale = 0.05f;
	float GSB_AO_bias = 0.05f;


	//16
	float GSB_AO_radius = 0.02f;
	float GSB_AO_offset = 0.707f;
	float padding1[2]{0};
};

using GraphicSettingsBuffer = ConstantBuffer<GraphicSettingsBufferData>;