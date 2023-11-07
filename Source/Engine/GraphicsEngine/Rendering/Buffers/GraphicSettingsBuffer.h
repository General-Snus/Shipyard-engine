#pragma once
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>  
#include "../../Shaders/Registers.h"

struct GraphicSettingsBufferData
{
	int GSB_ToneMap; // 4 
	float padding1[3] {0};
};

typedef ConstantBuffer<GraphicSettingsBufferData> GraphicSettingsBuffer;