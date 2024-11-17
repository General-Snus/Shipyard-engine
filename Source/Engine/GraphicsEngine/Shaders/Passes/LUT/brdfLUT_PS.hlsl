 
#include "../../Headers/PBRFunctions.hlsli"

float2 main(SS_VStoPS input) : SV_Target
{ 
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);  
    return integratedBRDF;
}