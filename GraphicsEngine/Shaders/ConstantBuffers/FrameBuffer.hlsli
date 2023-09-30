#include "../Registers.h"

cbuffer FrameBuffer : register(HLSL_REG_FrameBuffer)
{
    float4x4 FB_InvView;
    float4x4 FB_Proj; 
    float FB_Time;
    float3 FB_CameraPosition;
    
    int FB_RenderMode;
    float3 padding;
}