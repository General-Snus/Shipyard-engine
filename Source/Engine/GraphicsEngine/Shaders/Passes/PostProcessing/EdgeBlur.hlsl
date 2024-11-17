#include "../../Headers/PBRFunctions.hlsli" 

PostProcessPixelOutput main(SS_VStoPS input)
{
    PostProcessPixelOutput result;
    const float gWeights[11] =
    {
        0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
    }; 
    
    int texWidth = 0;
    int texHeight = 0;
    int numMips = 0; 
    SSAOPass.GetDimensions(0, texWidth, texHeight, numMips); 
    
    const float pixelWidth = 1.0f / texWidth;
    const float pixelHeight = 1.0f / texHeight;
    const int gBlurRadius = 5;
     
    float4 color = gWeights[5] * SSAOPass.Sample(defaultSampler, input.UV);
    float totalWeight = gWeights[5];
    const float4 centerNormalDepth = float4(textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler, input.UV).rgb, 0);

    for (float y = -gBlurRadius; y <= gBlurRadius; ++y)
    { 
        for (float x = -gBlurRadius; x <= gBlurRadius; ++x)
        { 
            if (x == 0)
            {
                continue;
            }
            float2 tex = float2(input.UV.x + x * pixelWidth, input.UV.y + y * pixelHeight);
            float4 neighborNormalDepth = float4(textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler, tex).rgb, 0);
    
            if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f && abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
            {
                float weight = gWeights[x + gBlurRadius];
                color += weight * float4(SSAOPass.Sample(defaultSampler, tex).rgb, 0);
                totalWeight += weight;
            }
        } 
        if (y == 0)
        {
            continue;
        }
        float2 tex = input.UV + y * float2(0.0f, pixelHeight);
        float4 neighborNormalDepth = float4(textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler, tex).rgb, 0);
        if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f && abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
        {
            float weight = gWeights[y + gBlurRadius]; 
            color += weight * float4(SSAOPass.Sample(defaultSampler, tex).rgb, 0);
            totalWeight += weight;
        } 
    } 
    result.Color = color / totalWeight;   
    return result;
}