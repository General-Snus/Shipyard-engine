#include "GBufferOutput.hlsli"
#include "../../Headers/DefaultVertexToPixel.hlsli"
#include "../../Headers/MaterialBuffer.hlsli"
#include "../../Headers/FrameBuffer.hlsli"





GBufferOutput main(DefaultVertexToPixel input) : SV_TARGET
{
    GBufferOutput result;

    const float3x3 TBN = float3x3(
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - input.WorldPosition.xyz);
   // result.Color.rgb = (input.Normal.rgb + 1) / 2.0f;
    const float2 uv = input.UV * DefaultMaterial.UVTiling;
    
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
    
    result.Normal = pixelNormal;
    
    result.Material = materialComponent; 
    
    result.Effect = effect;
    
    result.VertexNormal = input.Normal;
    result.VertexNormal.w = 1;
    
    result.WorldPosition = input.WorldPosition;
    
    return result;
}