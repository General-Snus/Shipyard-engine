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
    const float2 uv = input.UV;
    
    const float4 textureColor = textureHeap[g_defaultMaterial.AlbedoTextureIndex].Sample(defaultSampler, uv); // * g_defaultMaterial.DefaultMaterial.albedoColor;
    
    if(textureColor.a < 0.1f)
    {
        discard;
    }
    
    const float4 materialComponent = textureHeap[g_defaultMaterial.MaterialTextureIndex].Sample(defaultSampler, uv);
    const float2 textureNormal = textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler, uv).xy;
    const float4 effect = textureHeap[g_defaultMaterial.EmissiveTextureIndex].Sample(defaultSampler, uv);

    //Normals
    float3 pixelNormal;
    pixelNormal.xy = ((2.0f * textureNormal.xy) - 1.0f);
    pixelNormal.z = sqrt(1 - (pow(pixelNormal.x, 2.0f) + pow(pixelNormal.y, 2.0f)));
    pixelNormal = normalize(mul(pixelNormal, TBN));
     pixelNormal *= g_defaultMaterial.NormalStrength;
     
    result.Albedo = textureColor;
    
    result.Normal.xyz = pixelNormal;
    result.Normal.w = 1;
    
    result.Material = materialComponent;
    
    result.Effect = effect;
    
    result.VertexNormal.xyz = input.Normal;
    result.VertexNormal.w = 1;
    
    result.WorldPosition = input.WorldPosition;
    result.Depth =  (length(cameraDirection));
    return result;
}