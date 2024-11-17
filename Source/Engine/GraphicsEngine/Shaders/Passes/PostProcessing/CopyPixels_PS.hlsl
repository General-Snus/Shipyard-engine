 #include "../../Headers/ShaderStructs.hlsli"
 

PostProcessPixelOutput main(SS_VStoPS input)
{
    PostProcessPixelOutput output;
    output.Color = Target0_Texture.Sample(defaultSampler, input.UV); 
    return output;
}