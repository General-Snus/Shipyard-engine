#include <DefaultVertexToPixel.hlsli>
#include <DefaultVertexOutput.hlsli>
#include <IBLBRDF.hlsli>

float2 main(BRDF_VS_to_PS input) : SV_Target
{ 
    float2 integratedBRDF = IntegrateBRDF(input.uv.x, input.uv.y); 
    
    return integratedBRDF;
}