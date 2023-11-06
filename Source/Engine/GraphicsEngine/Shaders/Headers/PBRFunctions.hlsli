#include "../Registers.h" 
#include "../Headers/ShaderStructs.hlsli"  
#include "../Headers/Common.hlsli"  
 
float2 GetRandom(float2 uv, float2 uvScale)
{
    const float3 random = 2.0f * Noise_Texture.Sample(PointSampler, uv * uvScale).rgb - 1.0f;
    return random.xy;
}

float4 GetViewPosition(float2 uv)
{
    const float4 worldPosition = float4(worldPositionMap.Sample(defaultSampler, uv).rgb, 1);
    const float4 viewPosition = mul(FB_InvView, worldPosition);
    return viewPosition;
}


float4 GetViewNormal(float2 uv)
{
    const float4 worldNormal = float4(normalMap.Sample(defaultSampler, uv).rgb, 0);
    const float4 viewNormal = mul(FB_InvView, worldNormal);
    return viewNormal;
}
float3 CalculateDiffuseIBL(float3 normal, TextureCube enviromentCube)
{
    const int CubeMips = GetNumMips(enviromentCube) - 1;
    const float3 cubeMap = enviromentCube.SampleLevel(defaultSampler, normal, CubeMips).rgb;
    
    return cubeMap;
}

float3 CalculateSpecularIBL(float3 specularColor, float3 normal, float3 cameraDirection, float roughness, TextureCube enviromentCube)
{
    const int CubeMips = GetNumMips(enviromentCube) - 1;
    const float3 ReflectionVector = reflect(-cameraDirection, normal);
    const float3 cubeMap = enviromentCube.SampleLevel(defaultSampler, ReflectionVector, roughness * CubeMips).rgb;
    
    const float NdotV = saturate(dot(normal, cameraDirection));
    const float2 brdfLUT = BRDF_LUT_Texture.Sample(BRDFSampler, float2(NdotV, roughness)).xy;
    
    return cubeMap * (specularColor * brdfLUT.x + brdfLUT.y);;
}

float3 CalculateDiffuseLight(float3 diffuseColor)
{
    return (diffuseColor / PI);

}
float3 CalculateSpecularLight(float3 specularColor, float3 normal, float3 cameraDirection, float3 lightDirection, float3 halfAngle, float roughness)
{
    const float3 G = GeometricAttenuation_Smith_IBL(normal, cameraDirection, lightDirection, roughness);
     
    const float3 D = pow(roughness, 4) / (PI * pow((pow(dot(normal, halfAngle), 2) * (pow(roughness, 4) - 1) + 1), 2));
    
    const float fresnel = ((-5.55373 * saturate(dot(cameraDirection, halfAngle)) - 6.981316) * saturate(dot(cameraDirection, halfAngle)));
    const float3 F = specularColor + (1 - specularColor) * pow(2, fresnel);
    const float3 B = 4 * saturate(dot(normal, lightDirection)) * saturate(dot(normal, cameraDirection));
    
    const float3 directLightSpecular = (D * F * G) / B;
    
    return directLightSpecular;
}

#define definedPBR


