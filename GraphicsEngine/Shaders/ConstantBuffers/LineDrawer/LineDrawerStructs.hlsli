#include "../../Registers.h"

struct LineVertexInput
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct LineVertexToPixel
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

struct LinePixelOutput
{
    float4 Color : SV_TARGET;
};

cbuffer LineBuffer : register(HLSL_REG_LineBuffer)
{
    float4x4 LB_Transform;
}