#include "../Registers.h"
#include "LightsData.hlsli"
#include "MaterialBuffer.hlsli"
#include "ObjectBuffer.hlsli"


struct VertexOutput
{
    float4 Position : SV_POSITION; // carry till geo 
};
struct GSOutput
{
    float4 posCS : SV_POSITION; 
};

struct GBufferOutput
{
    float4 Albedo : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Material : SV_TARGET2;
    float4 Effect : SV_TARGET3;
    float4 VertexNormal : SV_TARGET4;
    float4 WorldPosition : SV_TARGET5;
    float4 Depth : SV_TARGET6;
};

struct DefaultVertexInput
{
    float4 Position : POSITION;
    float4 VxColor : COLOR;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    uint4 BoneIds : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
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


struct BRDF_VS_to_PS
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

struct DefaultPixelOutput
{
    float4 Color : SV_TARGET;
};

struct BRDF_Result
{
    float2 color : SV_Target;
}; 

cbuffer FrameBuffer : register(HLSL_REG_FrameBuffer)
{
    float4x4 FB_InvView;
    float4x4 FB_Proj;
    float FB_Time;
    float3 FB_CameraPosition;
    
    int FB_RenderMode;
    float3 padding;
}

cbuffer LightBuffer : register(HLSL_REG_LightBuffer)
{
    DirectionalLight myDirectionalLight; //32
    PointLight myPointLight; //32
    SpotLight mySpotLight; //64
}

cbuffer DefaultMaterialBuffer : register(HLSL_REG_DefaultMaterialBuffer)
{
    DefaultMaterialData DefaultMaterial;
};


cbuffer ObjectBuffer : register(HLSL_REG_ObjectBuffer)
{
    float4x4 OB_Transform; // 64 bytes
    float3 OB_MinExtents; // 12 bytes
    float padding1; // 4 bytes
    float3 OB_MaxExtents; // 12 bytes
    bool hasBone; // 4 bytes
    float4x4 OB_BoneTransform[128]; //64*128  
}


SamplerState defaultSampler : register(HLSL_REG_DefaultSampler);
SamplerState BRDFSampler : register(HLSL_REG_BRDFSampler);

Texture2D colorMap : register(HLSL_REG_colorMap);
Texture2D normalMap : register(HLSL_REG_normalMap);
Texture2D materialMap : register(HLSL_REG_materialMap);
Texture2D effectMap : register(HLSL_REG_effectMap);
Texture2D vertexNormalMap : register(HLSL_REG_VertexNormal);
Texture2D worldPositionMap : register(HLSL_REG_WorldPosition);
Texture2D DepthMap : register(HLSL_REG_DepthMap);

TextureCube enviromentCube : register(HLSL_REG_enviromentCube);
Texture2D BRDF_LUT_Texture : register(HLSL_REG_BRDF_LUT_Texture);

SamplerComparisonState shadowCmpSampler : register(HLSL_REG_shadowCmpSampler);
Texture2D shadowMap : register(HLSL_REG_dirLightShadowMap);

#define DefinedSamplers