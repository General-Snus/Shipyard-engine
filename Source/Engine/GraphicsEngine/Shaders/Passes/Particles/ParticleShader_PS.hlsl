 #include "../../Headers/ParticleStruct.hlsli"
 #include "../../Headers/ShaderStructs.hlsli"

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
    ParticlePixelOutput output;
    output.Color = colorMap.Sample(defaultSampler, input.UV) * input.Color;
    
    if(input.Color.a < 0.1f)
    {   
        discard;
    }
    
    return output;
}