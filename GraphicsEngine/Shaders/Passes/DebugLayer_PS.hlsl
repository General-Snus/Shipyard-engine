 
#include "GBuffer/GBufferPS.hlsl"

SamplerComparisonState shadowCmpSampler : register(HLSL_REG_shadowCmpSampler);
Texture2D shadowMap : register(HLSL_REG_dirLightShadowMap);

DefaultPixelOutput main(BRDF_VS_to_PS input) : SV_TARGET
{
    
    DefaultPixelOutput result;
    float2 uv = input.uv;
    
    const float4 albedo = colorMap.Sample(defaultSampler, uv);
    const float4 Material = materialMap.Sample(defaultSampler, uv);
    const float4 Normal = normalMap.Sample(defaultSampler, uv);
    const float4 Effect = effectMap.Sample(defaultSampler, uv);
    const float4 vertexNormal = vertexNormalMap.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(worldPositionMap.Sample(defaultSampler, uv).xyz, 1);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r;
    
    
#if _DEBUG
    switch(FB_RenderMode)
    {
    default:
    case 0:
    {
    break;
    }
    
    case 1:
    {
    result.Color.rgb = worldPosition.xyz;
    result.Color.a = 1.0f;
    break;
    }
    
     case 2:
    {
    result.Color.rgb = vertexNormal;
    result.Color.a = 1.0f;
    break;
    }
    
    case 3:
    {
    result.Color.rgb = albedo;
    result.Color.a = 1.0f;
    break;
    }
    
    case 4:
    {
    result.Color.rgb = Normal;
    result.Color.a = 1.0f;
    break;
    } 
    
    case 5:
    {
    result.Color.rgb = 1;
    result.Color.a = 1.0f;
    break;
    } 
    
    case 6:
    {
    result.Color.rgb = occlusion;
    result.Color.a = 1.0f;
    break;
    } 
    
     case 7:
    {
    result.Color.rgb = roughness;
    result.Color.a = 1.0f;
    break;
    } 
    
    
     case 8:
    {
    result.Color.rgb = metallic;
    result.Color.a = 1.0f;
    break;
    } 
    
     case 9:
    {
    result.Color.rgb = Effect.r;
    result.Color.a = 1.0f;
    break;
    } 
     case 10:
    {
    result.Color.rgb = 1;
    result.Color.a = 1.0f;
    break;
    } 
     case 11:
    {
    result.Color.rgb = 1;
    result.Color.a = 1.0f;
    break;
    } 
     case 12:
    {
    result.Color.rgb = 1;
    result.Color.a = 1.0f;
    break;
    } 
     case 13:
    {
    result.Color.rgb = 1;
    result.Color.a = 1.0f;
    break;
    } 
    
} 
#endif
    
    
    return result;
}