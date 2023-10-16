 #include "../../Headers/ShaderStructs.hlsli"
 

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput output;
    output.Color.rgb = Target0_Texture.Sample(defaultSampler, input.UV).rgb;
    output.Color.a = 1;
    return output;
}