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
    const float intensity = 0.35f;
    const float scale = 0.05f;
    const float bias = 0.05f;
    const float radius = 0.002f;
    const float offset = 0.707f;
    
    int noiseWidth = 0;
    int noiseHeight = 0;
    int numMips = 0;
    Noise_Texture.GetDimensions(0, noiseWidth, noiseHeight, numMips); 
    
    const float2 randomUVScale = FB_ScreenResolution / float2(noiseWidth, noiseHeight);
    const float2 random = GetRandom(input.UV,randomUVScale);
    const float4 pxPos = GetViewPosition(input.UV);
    const float4 pxNorm = GetViewNormal(input.UV);
    const float2 frustrumCorners[4] = {float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1)};
    
    float occlusion = 0.0f;
    for(uint i = 0; i < 4; i++)
    {
        const uint idx = i % 4;
        const float2 coord1 = reflect(frustrumCorners[idx],random) * radius;
        const float2 coord2 = float2((coord1.x - coord1.y) * offset, (coord1.x + coord1.y) * offset);
        
        occlusion += SSAO(input.UV, coord1*0.25f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord1*0.50f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord1*0.75f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        occlusion += SSAO(input.UV, coord1*1.00f, pxPos.xyz, pxNorm.xyz, scale, bias, intensity);
        
    }
    result.Color.rgb = 1 - occlusion;
    result.Color.a = 1.0f;
    return result;
}