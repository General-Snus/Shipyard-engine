struct DefaultMaterialData
{
    float4 albedoColor;
    float2 UVTiling;
    float NormalStrength;
    float Shine;
    float Roughness;
};

cbuffer DefaultMaterialBuffer : register(b2)
{
    DefaultMaterialData DefaultMaterial;
};