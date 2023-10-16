#include "../../Headers/ShaderStructs.hlsli"

PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput output;
    output.Color = float4(0, 0, 0, 0);
    const float4 color = Target0_Texture.Sample(defaultSampler, input.UV);
    const float3 resource = color.rgb;
    
    /*
    if(color.a < 0.05)
    {
        return output;
    } 
    float luminance = dot(resource, float3(0.2123, 0.7142, 0.0722));
    output.Color.rgb = resource * pow(luminance, 5);
    output.Color.a = 1;
*/
    
    float luminance = dot(resource, float3(0.2123, 0.7142, 0.0722));
    float cutoff = 0.8f;
    if(luminance >= cutoff)
    {
        output.Color.rgb = resource;
    }
    else if (luminance >= cutoff*.5f)
    {
        float fade = luminance / cutoff;    
        fade = pow(fade, 5);
        output.Color.rgb = resource * fade;
    }
    else
    {
        output.Color.rgb = 0;
    }
     
    output.Color.a = 1;
    return output;
}