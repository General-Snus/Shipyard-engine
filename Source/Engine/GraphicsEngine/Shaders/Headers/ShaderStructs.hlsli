#ifndef SHADERSTRUCTS
#define SHADERSTRUCTS
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
    
    float4x4 World : WORLD;
    uint InstanceID : SV_InstanceID;
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

cbuffer FrameBuffer : register(HLSL_REG_FrameBuffer)
{
    float4x4 FB_InvView;
    float4x4 FB_Proj;
    float FB_Time;
    float3 FB_CameraPosition;
    
    int FB_RenderMode;
    int2 FB_ScreenResolution;
    float1 padding;
    float4 FB_FrustrumCorners[4];
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

cbuffer GraphicSettingsBuffer : register(HLSL_REG_GraphicSettingsBuffer)
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
}


cbuffer ObjectBuffer : register(HLSL_REG_ObjectBuffer)
{
    float4x4 OB_Transform; // 64 bytes
    float3 OB_MinExtents; // 12 bytes
    bool hasBone; // 4 bytes
    float3 OB_MaxExtents; // 12 bytes
    bool OB_Instanced;
    float4x4 OB_BoneTransform[128]; //64*128  
}

#define myTex2DSpace space1
#define myTexCubeSpace space2

Texture2D Texture2DTable[] : register(t0, myTex2DSpace);
TextureCube TextureCubeTable[] : register(t0, myTexCubeSpace);

SamplerState defaultSampler : register(HLSL_REG_DefaultSampler);
SamplerState BRDFSampler : register(HLSL_REG_BRDFSampler);
SamplerState NormalDepthSampler : register(HLSL_REG_normalDepthSampler);
SamplerState PointSampler : register(HLSL_REG_PointSampler);
SamplerComparisonState shadowCmpSampler : register(HLSL_REG_shadowCmpSampler);

 Texture2D colorMap : register(HLSL_REG_colorMap);
 Texture2D normalMap : register(HLSL_REG_normalMap);
 Texture2D materialMap : register(HLSL_REG_materialMap);
 Texture2D effectMap : register(HLSL_REG_effectMap);
 Texture2D vertexNormalMap : register(HLSL_REG_VertexNormal);
 Texture2D worldPositionMap : register(HLSL_REG_WorldPosition);
 Texture2D DepthMap : register(HLSL_REG_DepthMap);
 Texture2D SSAOMap : register(HLSL_REG_SSAO);
 
 TextureCube enviromentCube : register(HLSL_REG_enviromentCube);
 Texture2D Noise_Texture : register(HLSL_REG_Noise_Texture);
 Texture2D BRDF_LUT_Texture : register(HLSL_REG_BRDF_LUT_Texture);
 Texture2D Target0_Texture : register(HLSL_REG_Target0);
 Texture2D Target01_Texture : register(HLSL_REG_Target01);
 Texture2D Target02_Texture : register(HLSL_REG_Target02);
 Texture2D Target03_Texture : register(HLSL_REG_Target03);
 Texture2D Target04_Texture : register(HLSL_REG_Target04);
 Texture2D shadowMap : register(HLSL_REG_dirLightShadowMap); 
#endif
