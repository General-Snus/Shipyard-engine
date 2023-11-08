#include "../../Headers/ShaderStructs.hlsli"
#include "../../Headers/Tonemaps.hlsli"
#include "../../Headers/Common.hlsli"

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput output; 
    const float3 color = Target01_Texture.Sample(defaultSampler, input.UV).rgb;
    switch(GSB_ToneMap)
    {
        default:
            output.Color.rgb = LinearToGamma(color.rgb);
            break;
        case 1:
            output.Color.rgb = LinearToGamma(Tonemap_Reinhard2(color.rgb));
            break;
        
        case 2:
            output.Color.rgb = LinearToGamma(Tonemap_UnrealEngine(color.rgb));
            break;
        
        case 3:
            output.Color.rgb = LinearToGamma(Tonemap_ACES(color.rgb));
            break;
        
        case 4:
            output.Color.rgb = LinearToGamma(Tonemap_Lottes(color.rgb));
            break;

    } 
    output.Color.a = 1;
    return output;
}