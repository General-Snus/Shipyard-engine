#include "../../Registers.h" 


SamplerState defaultSampler : register(HLSL_REG_DefaultSampler);
SamplerState BRDFSampler : register(HLSL_REG_BRDFSampler);
Texture2D colorMap : register(HLSL_REG_colorMap);
Texture2D normalMap : register(HLSL_REG_normalMap);
Texture2D materialMap : register(HLSL_REG_materialMap);
Texture2D effectMap : register(HLSL_REG_effectMap);
TextureCube enviromentCube : register(HLSL_REG_enviromentCube);
Texture2D BRDF_LUT_Texture : register(HLSL_REG_BRDF_LUT_Texture);

struct GBufferOutput
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Material: SV_Target2;
    float4 Effect: SV_Target3;
    
    float4 VertexNormal: SV_Target4;
    
    float4 WorldPosition: SV_Target5;
    
    float4 depth : SV_Target6;
};