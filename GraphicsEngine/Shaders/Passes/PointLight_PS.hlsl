#include "GBuffer/GBufferPS.hlsl"

#include "../Headers/LightBuffer.hlsli"
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

float3 CalculatePointLight(float3 diffuseColor, float3 specularColor, float3 worldPosition, float3 normal, float3 cameraDirection, PointLight pointLightData, float roughness, float shine)
{
    float3 lightDirection = (pointLightData.Position - worldPosition);
    const float distance = length(lightDirection);
    
    lightDirection = normalize(lightDirection);
    const float3 halfAngle = normalize(cameraDirection + lightDirection);
    
    const float constantAtt = 1.0f;
    const float linearAtt = 4.6 * pow(distance, -1);
    const float quadraticAtt = 8.9 * pow(distance, -0.5);
    
    const float3 Attenuation = 1 / (constantAtt + distance * (linearAtt + quadraticAtt * distance));
    
    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, lightDirection, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    const float NdotL = dot(lightDirection, normal);
    
    return saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * pointLightData.Color * pointLightData.Power;
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
    
    float3 totalPointLightContribution = 0;
     
        [flatten]
    if (myPointLight.Power > 0)
    {
        totalPointLightContribution += CalculatePointLight(
            diffuseColor,
            specularColor,
            worldPosition.xyz,
            Normal.xyz,
            cameraDirection,
            myPointLight,
            roughness,
            DefaultMaterial.Shine            
            );
    }
   
    const float3 radiance = totalPointLightContribution;
    result.Color.rgb = (radiance ) * albedo.rgb;
    
    //result.Color.rgb = saturate(LinearToGamma(result.Color.rgb));
    result.Color.a = 1.0f;
    
    return result;
}