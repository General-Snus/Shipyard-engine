#include "../../Headers/ShaderStructs.hlsli"
#include "../../Headers/PBRFunctions.hlsli"

float2 main(BRDF_VS_to_PS input) : SV_Target
{ 
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);  
    return integratedBRDF;
}