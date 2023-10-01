#include "../Registers.h"

struct DefaultMaterialData
{
    float4 albedoColor;
    float2 UVTiling;
    float NormalStrength;
    float Shine;
    float Roughness;
};

cbuffer DefaultMaterialBuffer : register(HLSL_REG_DefaultMaterialBuffer)
{
    DefaultMaterialData DefaultMaterial;
};