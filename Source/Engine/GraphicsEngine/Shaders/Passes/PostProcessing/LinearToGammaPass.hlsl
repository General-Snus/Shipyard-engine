#include "../../Headers/ShaderStructs.hlsli"
#include "../../Headers/Common.hlsli"

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput output; 
    const float4 color = Target01_Texture.Sample(defaultSampler, input.UV);
    output.Color.rgb = saturate(LinearToGamma(color.rgb));
    output.Color.a = color.a;
    return output;
}