#include "../Registers.h" 
#ifndef MaterialSTRUCTS
#define MaterialSTRUCTS 

struct DefaultMaterialBuffer
{
    float4 albedoColor;

    float2 UVTiling;
    float NormalStrength;
    float Shine; 
    float Roughness;

    int albedoTexture;
    int normalTexture;
    int MaterialTexture; 
    int emissiveTexture;

    uint vertexOffset;
    uint vertexBufferIndex; 

    float1 padding;
};
ConstantBuffer<DefaultMaterialBuffer> g_defaultMaterial : register(HLSL_REG_DefaultMaterialBuffer);

#endif
