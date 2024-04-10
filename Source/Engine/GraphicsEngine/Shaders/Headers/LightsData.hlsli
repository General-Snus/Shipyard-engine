#include "../Registers.h" 
#ifndef LIGHTSTRUCTS
#define LIGHTSTRUCTS
struct hlslDirectionalLight
{
    float3 Color; //12
    float Power; //4
    
    float4 Direction;
    
    float4x4 lightView;
    float4x4 projection;

    int shadowMapIndex;
    float3 padding;
};

struct hlslPointLight
{
    float3 Color; //12
    float Power; //4
    
    float3 Position; //12
    float Range; //4  

    float4x4 lightView;
    float4x4 projection;

    int shadowMapIndex[6];

};

struct hlslSpotLight
{
    float3 Color; //12
    float Power; //4
    
    float3 Position; //12
    float Range; //4
    
    float3 Direction; //12
    float InnerConeAngle; //4
    
    float OuterConeAngle; //4
    int shadowMapIndex;
    float2 Pad; //12
    
    float4x4 lightView;
    float4x4 projection;
};
struct LightBuffer
{
    int pointLightAmount;
    int spotLightAmount;
    float2 padding;

    hlslDirectionalLight myDirectionalLight; //32
    hlslPointLight myPointLight[8]; //32
    hlslSpotLight mySpotLight[8]; //64
};
ConstantBuffer<LightBuffer> g_lightBuffer : register(HLSL_REG_LightBuffer);

#endif
