#include "../Registers.h"
#include "../Headers/ShaderStructs.hlsli"  
 

float Sine(float aValue)
{
    const float pi2 = 6.28318548f;
    const float shiftedValue = aValue * pi2;
    return sin(shiftedValue);
}

float3 PositionInBound(float3 aMin, float3 aMax, float3 aPosition)
{
    const float3 boundSize = aMax - aMin;
    return aPosition / boundSize;
}

DefaultPixelOutput main(DefaultVertexToPixel input)
{
    DefaultPixelOutput result; 
    result.Color.rgb = textureHeap[0].Sample(defaultSampler, input.UV).rgb;
    result.Color.a = 1.0f; 
    return result;
}