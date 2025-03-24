 #include "../../Headers/ShaderStructs.hlsli"
 
PostProcessPixelOutput main(SS_VStoPS input)
{
    PostProcessPixelOutput output;
    
    const float3 color1 = Target0_Texture.Sample(defaultSampler, input.UV).rgb;
    const float3 color2 = Target01_Texture.Sample(defaultSampler, input.UV).rgb;
    //color1 += Target0_Texture.SampleLevel(defaultSampler, input.UV, 0, 1.5).rgb;
    //color1 += Target0_Texture.SampleLevel(defaultSampler, input.UV, 0, 2.5).rgb;
    //color1 /= 3;
    
    
    //Additive
    //output.Color.rgb = color1 + color2;
     
    //Scaled
    //float3 scaledResource = color1 * (1 - saturate(color2));
    //output.Color.rgb = scaledResource * color2;
    
    
    //LumnanceBased
    const float luminance = dot(color1, float3(0.2126, 0.7152, 0.0722));
    const float3 scaledResource = color2 * (1 - luminance);
    output.Color.rgb = scaledResource + color1;
    
    
    output.Color.a = 1;
    return output;
}