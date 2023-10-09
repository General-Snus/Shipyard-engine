 
#include "../../Headers/DefaultVertexToPixel.hlsli"
#include "../../Headers/DefaultVertexOutput.hlsli"
#include "../../Headers/MaterialBuffer.hlsli"
#include "../../Headers/FrameBuffer.hlsli" 
#include "../../Headers/IBLBRDF.hlsli"

#include "../../Registers.h" 


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

GBufferOutput main(DefaultVertexToPixel input)
{
    GBufferOutput result;
    const float3x3 TBN = float3x3(
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - input.WorldPosition.xyz);
   // result.Color.rgb = (input.Normal.rgb + 1) / 2.0f;
    const float2 uv = input.UV ;
    
    const float4 textureColor = colorMap.Sample(defaultSampler, uv) * DefaultMaterial.albedoColor;
    const float4 materialComponent = materialMap.Sample(defaultSampler, uv);
    const float2 textureNormal = normalMap.Sample(defaultSampler, uv).xy;
    const float4 effect = effectMap.Sample(defaultSampler, uv);
    
    //const float occlusion = materialComponent.r;
    //const float roughness = materialComponent.g;
    //const float metallic = materialComponent.b;
    
    
    //Normals
    float3 pixelNormal;
    pixelNormal.xy = ((2.0f * textureNormal.xy) - 1.0f);
    pixelNormal.z = sqrt(1 - (pow(pixelNormal.x, 2.0f) + pow(pixelNormal.y, 2.0f)));
    pixelNormal = normalize(mul(pixelNormal, TBN));
    pixelNormal *= DefaultMaterial.NormalStrength; 
     
    result.Albedo = textureColor;
    
    result.Normal.xyz = pixelNormal;
    result.Normal.w = 1;
    
    result.Material = materialComponent; 
    
    result.Effect = effect;
    
    result.VertexNormal.xyz = input.Normal;
    result.VertexNormal.w = 1;
    
    result.WorldPosition = input.WorldPosition;
    result.Depth = 1;
    return result;
}