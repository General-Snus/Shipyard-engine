#include "../../Headers/ShaderStructs.hlsli"  




GBufferOutput main(DefaultVertexToPixel input)
{
    GBufferOutput result;
    const float3x3 TBN = float3x3(
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    const float3 cameraDirection = g_FrameBuffer.FB_CameraPosition.xyz - input.WorldPosition.xyz;
    const float3 cameraNormalizedDirection = normalize(cameraDirection);
   // result.Color.rgb = (input.Normal.rgb + 1) / 2.0f;
    const float2 uv = input.UV;
    
    const float4 textureColor = colorMap.Sample(defaultSampler, uv); // * g_defaultMaterial.DefaultMaterial.albedoColor;
    
   if(textureColor.a < 0.1f)
   {
       discard;
   }
    
    //const float4 materialComponent = materialMap.Sample(defaultSampler, uv);
    //const float2 textureNormal = normalMap.Sample(defaultSampler, uv).xy;
   // const float4 effect = effectMap.Sample(defaultSampler, uv);
    
    //const float occlusion = materialComponent.r;
    //const float roughness = materialComponent.g;
    //const float metallic = materialComponent.b;
    
    
    //Normals
    //float3 pixelNormal;
    //pixelNormal.xy = ((2.0f * textureNormal.xy) - 1.0f);
    //pixelNormal.z = sqrt(1 - (pow(pixelNormal.x, 2.0f) + pow(pixelNormal.y, 2.0f)));
    //pixelNormal = normalize(mul(pixelNormal, TBN));
    //pixelNormal *= g_defaultMaterial.DefaultMaterial.NormalStrength;
     
    result.Albedo = textureColor;
    
    result.Normal.xyz = 0;
    result.Normal.w = 0;
    
    result.Material = 0;
    
    result.Effect = 0;
    
    result.VertexNormal.xyz = input.Normal;
    result.VertexNormal.w = 1;
    
    result.WorldPosition = input.WorldPosition;
    result.Depth = saturate(length(cameraDirection));
    return result;
}