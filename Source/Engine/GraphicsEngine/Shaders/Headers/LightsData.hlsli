#include "../Registers.h" 
#ifndef LIGHTSTRUCTS
#define LIGHTSTRUCTS


struct hlslSpotLight
{
    double ptrPadding;
    float3 Color;
    float Power;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float InnerConeAngle;
    float OuterConeAngle;
    int shadowMapIndex;
    bool castShadow;
    float padding;
    
    float4x4 lightView;
    float4x4 projection;
};

struct hlslPointLight
{
    double ptrPadding;
    float3 Color; //12
    float Power; //4
    
    float3 Position; //12
    float Range; //4  

    float4x4 lightView[6];
    float4x4 projection;

    bool castShadow;
    int shadowMapIndex[6];
};

struct hlslDirectionalLight
{
    double ptrPadding;
    float3 Color; //12
    float Power; //4
    
    float4 Direction;
    
    float4x4 lightView;
    float4x4 projection;

    int shadowMapIndex;
    bool castShadow;
    float2 padding;
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
