#include "../Registers.h"

struct TextureInspectionBuffer
{
    float4x4 ColorTransform;
    float4 ColorOffset;
    float4 Grid;
    float2 GridWidth;
    float PremultiplyAlpha;
    float DisableFinalAlpha;
    float3 BackgroundColor;
    bool ForceNearestSampling;
    float2 TextureSize;
};

ConstantBuffer<TextureInspectionBuffer> g_TextureInspectionBuffer : register(HLSL_REG_TextureInspectionBuffer);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv : TEXCOORD0;
};

sampler sampler0;
Texture2D texture0;

float4 main(PS_INPUT input) : SV_Target
{
    float2 uv;
    float2 texel = input.uv * g_TextureInspectionBuffer.TextureSize;
    if (g_TextureInspectionBuffer.ForceNearestSampling) 
        uv = (floor(texel) + float2(0.5, 0.5)) / g_TextureInspectionBuffer.TextureSize;
    else
        uv = input.uv;

    float2 texelEdge = step(texel - floor(texel), g_TextureInspectionBuffer.GridWidth);
    float isGrid = max(texelEdge.x, texelEdge.y);
    float4 ct = mul(g_TextureInspectionBuffer.ColorTransform, texture0.Sample(sampler0, uv)) + g_TextureInspectionBuffer.ColorOffset;
    ct.rgb = ct.rgb * lerp(1.0, ct.a, g_TextureInspectionBuffer.PremultiplyAlpha);
    ct.rgb += g_TextureInspectionBuffer.BackgroundColor * (1.0 - ct.a);
    ct.a = lerp(ct.a, 1.0, g_TextureInspectionBuffer.DisableFinalAlpha);
    ct = lerp(ct, float4(g_TextureInspectionBuffer.Grid.rgb, 1), g_TextureInspectionBuffer.Grid.a * isGrid);
    return ct;
}