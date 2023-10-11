#include "../../Headers/ShaderStructs.hlsli"

VertexOutput main(float4 worldPosition : POSITION)
{
    VertexOutput output;
    output.Position = worldPosition;  
    return output;
}