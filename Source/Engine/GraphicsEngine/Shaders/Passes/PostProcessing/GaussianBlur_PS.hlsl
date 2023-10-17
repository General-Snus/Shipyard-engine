 #include "../../Headers/ShaderStructs.hlsli"
 #include "../../Headers/Common.hlsli"
 
PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput output;
    //Values are fucked cuz it looks like shit with values from presentation
    const float Direction = 16.0f;
    const float Quality = 4.0f;
    const float Radius = 2.0f;
    
    const float2 rad = Radius / FB_ScreenResolution;

    float4 color = Target0_Texture.Sample(defaultSampler, input.UV);
    
    
    if(color.a < 0.05f)
    {
        output.Color = 0;
        discard; 
    }
    const float pi2 = PI * 2; 
    for(float d = 0.0f; d < pi2; d += pi2 / Direction)
    {
        for(float q = 1.0f / Quality; q <= 1.0f; q += 1.0f / Quality)
        {
            color += Target0_Texture.Sample(defaultSampler, input.UV + 
            float2(cos(d), sin(d)) * rad * q);
        }
    }
    
    color /= Direction * Quality - 15.0f;
    output.Color.rgb = color.rgb;
    output.Color.a = 1.0f;
    
    return output;
}