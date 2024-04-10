 #include "../../Headers/ParticleStruct.hlsli"
 #include "../../Headers/ShaderStructs.hlsli"


ParticlePixelOutput main(ParticleGeometryToPixel input)
{
    ParticlePixelOutput output;
    output.Color = textureHeap[g_defaultMaterial.AlbedoTextureIndex].Sample(defaultSampler, input.UV);
    
    if(input.Color.a < 0.05f)
    {   
        discard;
    }
    output.Color = output.Color * input.Color;
    return output;
}