#include "../../Headers/ShaderStructs.hlsli"

BRDF_VS_to_PS main(unsigned int aVertexIndex : SV_VertexID)
{
    const float4 pos[6] =
    {
    float4(-1, -1, 0, 1),
    float4(-1, 1, 0, 1),
    float4(1, -1, 0, 1),
        
    float4(1, 1, 0, 1),
    float4(1, -1, 0, 1),
    float4(-1, 1, 0, 1)
    };
    
    const float2 uv[6] =
    {
    float2(0.0f, 1.0f), 
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
        
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 0.0f)
    };
    
    BRDF_VS_to_PS returnValue;
    returnValue.position = pos[aVertexIndex];
    returnValue.UV = uv[aVertexIndex];
    return returnValue;

}
