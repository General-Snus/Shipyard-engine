#include "../Registers.h"
#include "LightsData.hlsli"
#include "MaterialBuffer.hlsli"
#include "ObjectBuffer.hlsli"
#ifndef SHADERSTRUCTS
#define SHADERSTRUCTS

struct VertexOutput
{
    float4 Position : SV_POSITION; // carry till geo 
};
struct GSOutput
{
    float4 posCS : SV_POSITION;
};


struct Vertex
{
    float4 Position : POSITION;
    float4 VxColor : COLOR;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct DefaultVertexInput
{
    uint vertexID : SV_VertexID;
    uint instanceID : SV_InstanceID;
};
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
struct PostProcessVertexToPixel
{
    float2 UV : UV;
};
struct BRDF_VS_to_PS
{
    float4 position : SV_POSITION;
    float2 UV : UV;
};
struct DefaultPixelOutput
{
    float4 Color : SV_TARGET;
};
struct PostProcessPixelOutput
{
    float4 Color : SV_TARGET;
};
struct BRDF_Result
{
    float2 color : SV_Target;
};


struct FrameBuffer
{
    float4x4 FB_InvView;
    float4x4 FB_Proj;
    float FB_Time;
    float3 FB_CameraPosition;
    int FB_RenderMode;
    uint2 FB_ScreenResolution;
    float1 padding;
    //float4 FB_FrustrumCorners[4];
};
ConstantBuffer<FrameBuffer> g_FrameBuffer : register(HLSL_REG_FrameBuffer); 

struct GraphicSettingsBuffer
{
    // 16
    int GSB_ToneMap;
    float GSB_AO_intensity;
    float GSB_AO_scale;
    float GSB_AO_bias;
    
    //16
    float GSB_AO_radius;
    float GSB_AO_offset;
    float2 padding1;
};
ConstantBuffer<GraphicSettingsBuffer> g_GraphicsSettings : register(HLSL_REG_GraphicSettingsBuffer);

SamplerState defaultSampler : register(HLSL_REG_DefaultSampler);
SamplerState BRDFSampler : register(HLSL_REG_BRDFSampler);
SamplerState NormalDepthSampler : register(HLSL_REG_normalDepthSampler);
SamplerState PointSampler : register(HLSL_REG_PointSampler);
SamplerComparisonState shadowCmpSampler : register(HLSL_REG_shadowCmpSampler);

Texture2D textureHeap[] : register(HLSL_REG_colorMap);
ByteAddressBuffer meshHeap[] : register(HLSL_REG_colorMap,space4);
 


Texture2D colorPass : register(HLSL_REG_colorMap, space1);
Texture2D normalPass : register(HLSL_REG_normalMap, space1);
Texture2D materialPass : register(HLSL_REG_materialMap, space1);
Texture2D effectPass : register(HLSL_REG_effectMap, space1);
Texture2D vertexNormalPass : register(HLSL_REG_VertexNormal, space1);
Texture2D worldPositionPass : register(HLSL_REG_WorldPosition, space1);
Texture2D DepthPass : register(HLSL_REG_DepthMap, space1);
Texture2D SSAOPass : register(HLSL_REG_SSAO, space1);

Texture2D<float4> Target0_Texture : register(HLSL_REG_Target0, space2);
Texture2D<float4> Target01_Texture : register(HLSL_REG_Target01, space2);
Texture2D<float4> Target02_Texture : register(HLSL_REG_Target02, space2);
Texture2D<float4> Target03_Texture : register(HLSL_REG_Target03, space2);
Texture2D<float4> Target04_Texture : register(HLSL_REG_Target04, space2);
Texture2D<float4> shadowMap : register(HLSL_REG_dirLightShadowMap, space2);

TextureCube environmentCube : register(HLSL_REG_enviromentCube,space3);
Texture2D<float4> BRDF_LUT_Texture : register(HLSL_REG_BRDF_LUT_Texture, space3);
Texture2D<float4> Noise_Texture : register(HLSL_REG_Noise_Texture, space3);
#endif
