#include "../../Headers/PBRFunctions.hlsli" 

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput result;
    const float gWeights[11] =
    {
        0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
    }; 
    
    int texWidth = 0;
    int texHeight = 0;
    int numMips = 0; 
    SSAOMap.GetDimensions(0, texWidth, texHeight, numMips); 
    
    const float gTexelWidth = 1.0f / texWidth;
    const float gTexelHeight = 1.0f / texHeight;
    const int gBlurRadius = 5;
     
    float4 color = gWeights[5] * SSAOMap.Sample(NormalDepthSampler, input.UV);
    float totalWeight = gWeights[5];
    const float4 centerNormalDepth = float4(normalMap.Sample(NormalDepthSampler, input.UV).rgb, 0);

    for (float j = -gBlurRadius; j <= gBlurRadius; ++j)
    { 
        for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
        { 
            if (i == 0)
            {
                continue;
            }
            float2 tex = float2(input.UV.x + i * gTexelWidth, input.UV.y + j * gTexelHeight);
            float4 neighborNormalDepth = float4(normalMap.Sample(NormalDepthSampler, tex).rgb, 0);
    
            if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f && abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
            {
                float weight = gWeights[i + gBlurRadius];
                color += weight * float4(SSAOMap.Sample(NormalDepthSampler, tex).rgb, 0);
                totalWeight += weight;
            }
        } 
        if (j == 0)
        {
            continue;
        }
        float2 tex = input.UV + j * float2(0.0f, gTexelHeight);
        float4 neighborNormalDepth = float4(normalMap.Sample(NormalDepthSampler, tex).rgb, 0);
        if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f && abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
        {
            float weight = gWeights[j + gBlurRadius]; 
            color += weight * float4(SSAOMap.Sample(NormalDepthSampler, tex).rgb, 0);
            totalWeight += weight;
        } 
    } 
    result.Color = color / totalWeight;  
    result.Color.a = 1.0f;
    return result;
}