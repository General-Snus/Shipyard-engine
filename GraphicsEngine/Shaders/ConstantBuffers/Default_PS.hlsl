#include <DefaultVertexToPixel.hlsli>
#include <DefaultVertexOutput.hlsli>
#include <FrameBuffer.hlsli>
#include <MaterialBuffer.hlsli>
#include <IBLBRDF.hlsli>
#include "../Registers.h"

SamplerState defaultSampler : register(HLSL_REG_DefaultSampler);
SamplerState BRDFSampler : register(HLSL_REG_BRDFSampler);
Texture2D colorMap : register(HLSL_REG_colorMap);
Texture2D normalMap : register(HLSL_REG_normalMap);
Texture2D materialMap : register(HLSL_REG_materialMap);
TextureCube enviromentCube : register(HLSL_REG_enviromentCube);
Texture2D BRDF_LUT_Texture : register(HLSL_REG_BRDF_LUT_Texture);

float Sine(float aValue)
{
    const float pi2 = 6.28318548f;
    const float shiftedValue = aValue * pi2;
    return sin(shiftedValue);
}

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



float3 CalculateIndirectLight(
float3 diffuseColor,
float3 specularColor,
float3 normal,
float3 cameraDirection,
TextureCube enviromentCube,
float roughness,
float occulusion
)
{
    const float3 diffuse = CalculateDiffuseIBL(normal, enviromentCube);
    const float3 specular = CalculateSpecularIBL(specularColor, normal, cameraDirection, roughness, enviromentCube);
    
    const float3 kA = (diffuseColor * diffuse + specular) * occulusion;
    
    return kA;
}


float3 CalculateDirectionLight(
float3 diffuseColor,
float3 specularColor,
float3 normal,
float3 halfAngle,
float3 cameraDirection,
float3 lightDirection,
float3 lightColor,
float roughness
)
{
    const float NdotL = saturate(dot(normal, lightDirection));
    const float K = pow(roughness + 1, 2) / 8;
     
    const float3 G = GeometricAttenuation_Smith_IBL(normal, cameraDirection, lightDirection, roughness);

    
    const float3 D = pow(roughness, 4) / (PI * pow((pow(dot(normal, halfAngle), 2) * (pow(roughness, 4) - 1) + 1), 2));
    
    const float fresnel = ((-5.55373 * saturate(dot(cameraDirection, halfAngle)) - 6.981316) * saturate(dot(cameraDirection, halfAngle)));
    const float3 F = specularColor + (1 - specularColor) * pow(2, fresnel);
    const float3 B = 4 * saturate(dot(normal, lightDirection)) * saturate(dot(normal, cameraDirection));
    
    const float3 directLightSpecular = (D * F * G) / B;
    float3 directLightDiffuse = (diffuseColor / PI);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    return saturate(directLightDiffuse + directLightSpecular) * lightColor * NdotL;
}

DefaultPixelOutput main(DefaultVertexToPixel input)
{
    DefaultPixelOutput result;

    const float3x3 TBN = float3x3(
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - input.WorldPosition.xyz);
   // result.Color.rgb = (input.Normal.rgb + 1) / 2.0f;
    const float2 uv = input.UV * DefaultMaterial.UVTiling;
    
    const float4 textureColor = colorMap.Sample(defaultSampler, uv) * DefaultMaterial.albedoColor;
    const float4 materialComponent = materialMap.Sample(defaultSampler, uv);
    float3 pixelNormal = normalMap.Sample(defaultSampler, uv).xyz;
    
    const float occlusion = materialComponent.r;
    const float roughness = materialComponent.g;
    const float metallic = materialComponent.b;
    
    
    //Normals
    pixelNormal.xy = ((2.0f * pixelNormal.xy) - 1.0f);
    pixelNormal.z = sqrt(1 - (pow(pixelNormal.x, 2.0f) + pow(pixelNormal.y, 2.0f)));
    pixelNormal = normalize(mul(pixelNormal, TBN));
    pixelNormal *= DefaultMaterial.NormalStrength;
    
    //Enviroment
   
    
    
    
    float3 lightDirection = normalize(float3(1, 1, 1));
    float ambienceStrength = .1f;
    float3 lightColor = float3(1.0f, 1.0f, 1.0f) * 10;
    float3 materialSpecular = float3(1.0f, 1.0f, 1.0f);
    
    const float3 diffuseColor = lerp((float3) 0.0f, textureColor.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3) 0.04f, textureColor.rgb, metallic);
    
    const float3 halfAngle = normalize(cameraDirection + lightDirection);
     
    const float3 radiance =
    CalculateDirectionLight(diffuseColor, specularColor, pixelNormal.xyz, halfAngle, cameraDirection, lightDirection, lightColor, roughness) +
    CalculateIndirectLight(diffuseColor, specularColor, pixelNormal.xyz, cameraDirection, enviromentCube, roughness, occlusion);
    
    result.Color.rgb = radiance * textureColor.rgb;
  
    
    result.Color.rgb = saturate(LinearToGamma(result.Color.rgb));
    result.Color.a = 1.0f;
    
    return result;
}