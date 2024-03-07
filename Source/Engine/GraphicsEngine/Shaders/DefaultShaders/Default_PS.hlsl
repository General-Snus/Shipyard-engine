#include "../Registers.h" 
struct DefaultVertexToPixel
{
    float4 Position : SV_POSITION;
    float4 VxColor : COLOR;
    float4 WorldPosition : WORLDPOSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
};

struct DefaultPixelOutput
{
    float4 Color : SV_TARGET;
};

struct ObjectBuffer
{
    float4x4 OB_Transform; // 64 bytes
    float3 OB_MinExtents; // 12 bytes
    bool hasBone; // 4 bytes
    float3 OB_MaxExtents; // 12 bytes
    bool OB_Instanced;
    float4x4 OB_BoneTransform[128]; //64*128  
};
ConstantBuffer<ObjectBuffer> g_ObjectBuffer : register(HLSL_REG_ObjectBuffer);


float Sine(float aValue)
{
    const float pi2 = 6.28318548f;
    const float shiftedValue = aValue * pi2;
    return sin(shiftedValue);
}

float3 PositionInBound(float3 aMin, float3 aMax, float3 aPosition)
{
    const float3 boundSize = aMax - aMin;
    return aPosition / boundSize;
}

DefaultPixelOutput main(DefaultVertexToPixel input)
{
    DefaultPixelOutput result; 
    result.Color.rgb = PositionInBound(g_ObjectBuffer.OB_MinExtents, g_ObjectBuffer.OB_MaxExtents, input.Position.xyz);
    result.Color.a = 1.0f; 
    return result;
}