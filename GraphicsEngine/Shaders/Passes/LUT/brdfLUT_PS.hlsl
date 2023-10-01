#include <../../Headers/DefaultVertexToPixel.hlsli>
#include <../../Headers/DefaultVertexOutput.hlsli>
#include <../../Headers/IBLBRDF.hlsli>

float2 main(BRDF_VS_to_PS input) : SV_Target
{ 
    float2 integratedBRDF = IntegrateBRDF(input.uv.x, input.uv.y);  
    return integratedBRDF;
}