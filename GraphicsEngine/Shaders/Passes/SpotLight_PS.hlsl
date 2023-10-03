#include "GBuffer/GBufferPS.hlsl"

float3 PositionInBound(float3 aMin, float3 aMax, float3 aPosition)
{
    const float3 boundSize = aMax - aMin;
    return aPosition / boundSize;
}

int GetNumMips(TextureCube mipMap)
{
    int iWidth = 0;
    int iHeight = 0;
    int iNumMips = 0;
    mipMap.GetDimensions(0, iWidth, iHeight, iNumMips);
    return iNumMips;
}

float3 LinearToGamma(float3 aColor)
{
    return pow(abs(aColor), 1.0f / 2.2f);
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

float spotFallof(float spectatorAngle, float umbralAngle, float penumbralAngle)
{
     [flatten]
    if (spectatorAngle > penumbralAngle)
        return 1;
    
     [flatten]
    if (spectatorAngle < umbralAngle)
        return 0;
    
    const float denom = spectatorAngle - umbralAngle;
    const float numer = (penumbralAngle - umbralAngle);
    const float conalAtt = pow((denom / numer), 4);
    return conalAtt;
}

float3 CalculateSpotLight(float3 diffuseColor, float3 specularColor, float3 worldPosition, float3 normal, float3 cameraDirection, SpotLight spotLightData, float roughness, float shine)
{
    float3 RayFromSpot = (spotLightData.Position - worldPosition);
    const float dist = length(RayFromSpot);
    RayFromSpot = normalize(RayFromSpot);
    const float3 halfAngle = normalize(normalize(cameraDirection) + RayFromSpot);
    const float3 spotlightDirection = normalize(spotLightData.Direction);
    
    const float spectatorAngle = dot(spotlightDirection, -RayFromSpot);
    const float umbralAngle = cos(clamp(spotLightData.OuterConeAngle / 2, 0.001f, PI));
    const float penumbralAngle = cos(clamp(spotLightData.InnerConeAngle / 2, 0.001f, PI - umbralAngle));
    const float conalAtt = spotFallof(spectatorAngle, umbralAngle, penumbralAngle);
    
  //  const float3 Attenuation = 1 - saturate(pow(dist * pow(max(spotLightData.Range, 0.000001f), -1), 2));
    const float3 Attenuation = conalAtt / pow(max(dist, spotLightData.Range), 2); //REFACTOR: PHYSICS i hate this
      
    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, RayFromSpot, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    const float NdotL = saturate(dot(RayFromSpot, normal));
    const float solidAngle = 2 * PI * (1 - .5f * (penumbralAngle + umbralAngle)); // Solid angle of a cone
    const float radiantIntensity = spotLightData.Power / solidAngle; // Arbitrary conversion from watts
    return saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * spotLightData.Color;
}

DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    DefaultPixelOutput result;
    float2 uv = input.uv;
    
    const float4 albedo = colorMap.Sample(defaultSampler, uv);
    const float4 Material = materialMap.Sample(defaultSampler, uv);
    const float4 Normal = normalMap.Sample(defaultSampler, uv);
    const float4 Effect = effectMap.Sample(defaultSampler, uv);
    const float4 worldPosition = worldPositionMap.Sample(defaultSampler, uv);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r;
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - worldPosition.xyz);
    const float3 diffuseColor = lerp((float3) 0.0f, albedo.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metallic);
     
    float3 totalSpotLightContribution = 0;
        [flatten]
    if (mySpotLight.Power > 0)
    {
        totalSpotLightContribution += CalculateSpotLight(
            diffuseColor,
            specularColor,
            worldPosition.xyz,
            Normal.xyz,
            cameraDirection,
            mySpotLight,
            roughness,
            DefaultMaterial.Shine            
            );
    }
   
    const float3 radiance = totalSpotLightContribution;
    
    result.Color.rgb = (radiance ) * albedo.rgb;
  
    
   // result.Color.rgb = saturate(LinearToGamma(result.Color.rgb));
    result.Color.a = 1.0f;
    
    return result;
}