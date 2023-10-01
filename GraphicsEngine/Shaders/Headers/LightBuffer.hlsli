#include "../Registers.h"

struct  DirectionalLight
{
    float3 Color;   //12
    float Power; //4
    
    float3 Direction; //12
    float Pad;        //4
    
    float4x4 lightView;
    float4x4 projection;
    
}; 

struct PointLight
{
    float3 Color; //12
    float Power;//4
    
    float3 Position; //12
    float Range;    //4
};

struct SpotLight
{
    float3 Color;   //12
    float Power; //4
    
    float3 Position; //12
    float Range;    //4
    
    float3 Direction;       //12
    float InnerConeAngle;   //4
    
    float OuterConeAngle; //4
    float3 Pad; //12
};

cbuffer LightBuffer : register(HLSL_REG_LightBuffer)
{
    DirectionalLight myDirectionalLight;    //32
    PointLight myPointLight[PointLightCount];             //32 * 8
    SpotLight mySpotLight[SpotLightCount];               //64*8
}