#include "../../Headers/PBRFunctions.hlsli" 

float SSAO(float2 screenUV, float2 uv, float3 pxViewPos, float3 pxViewNrm, float scale, float bias, float intensity)
{
    const float3 diff = GetViewPosition(screenUV + uv).xyz - pxViewPos;
    const float3 v = normalize(diff);
    const float d = length(diff) * scale;
    const float occ = max(0.0f, dot(pxViewNrm, v) - bias) * (1.0f / (1.0f + d)) * intensity;
    return occ;
}

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput result;
     
    const float intensity = GSB_AO_intensity;
    const float scale = GSB_AO_scale;
    const float bias = GSB_AO_bias;
    const float radius = GSB_AO_radius;
    const float offset = GSB_AO_offset;
    
    int noiseWidth = 0;
    int noiseHeight = 0;
    int numMips = 0;
    Noise_Texture.GetDimensions(0, noiseWidth, noiseHeight, numMips);
    
    const float2 randomUVScale = FB_ScreenResolution / float2(noiseWidth, noiseHeight);
    const float2 random = GetRandom(input.UV, randomUVScale);
    const float3 pxPos = GetViewPosition(input.UV);
    const float4 pxNorm = GetViewNormal(input.UV);
    const float2 frustrumCorners[4] = {FB_FrustrumCorners[0].xy, FB_FrustrumCorners[1].xy, FB_FrustrumCorners[2].xy, FB_FrustrumCorners[3].xy};
    //const float2 frustrumCorners[4] = {float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1)};
    
    float occlusion = 0.0f;  
    for(uint i = 0; i < 4; i++)
    {
        const uint idx = i % 4;
        const float2 coord1 = reflect(frustrumCorners[idx], random) * radius;
        const float2 coord2 = float2((coord1.x - coord1.y) * offset, (coord1.x + coord1.y) * offset);
        
        occlusion += SSAO(input.UV, coord1 * 0.25f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord2 * 0.50f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord1 * 0.75f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord2 * 1.00f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        
    }
    const float occludeTotal = 1 - occlusion;
   // result.Color.rgb = pow(occludeTotal, 4);
    result.Color.rgb =  occludeTotal ;
    result.Color.a = 1.0f;
    return result;
}