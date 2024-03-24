#include "../Registers.h" 
#ifndef MaterialSTRUCTS
#define MaterialSTRUCTS
struct DefaultMaterialData
{
    float4 albedoColor;
    float2 UVTiling;
    float NormalStrength;
    float Shine;
    float Roughness; 
    int albedoTexture ;
    int normalTexture  ;
    int materialTexture ;
    int emissiveTexture ;
};

struct DefaultMaterialBuffer
{
    DefaultMaterialData DefaultMaterial;
};
ConstantBuffer<DefaultMaterialBuffer> g_defaultMaterial : register(HLSL_REG_DefaultMaterialBuffer);

#endif
