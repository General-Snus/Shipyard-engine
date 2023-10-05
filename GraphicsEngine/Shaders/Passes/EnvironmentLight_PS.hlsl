 
#include "GBuffer/GBufferPS.hlsl"

SamplerComparisonState shadowCmpSampler : register(HLSL_REG_shadowCmpSampler);
Texture2D shadowMap : register(HLSL_REG_dirLightShadowMap);

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
float3 cameraDirection,
float roughness,
float4 worldPosition
)
{
    const float3 directionToLight = -myDirectionalLight.Direction;
    const float3 halfAngle = normalize(cameraDirection + directionToLight);
    const float NdotL = saturate(dot(normal, directionToLight));
    const float K = pow(roughness + 1, 2) / 8;

    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, directionToLight, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    float4 lightSpacePos = (myDirectionalLight.lightView, worldPosition);
    lightSpacePos = mul(myDirectionalLight.projection, lightSpacePos);
    float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
    float D = lightSpaceUV.z    ;
    lightSpaceUV.xy = lightSpaceUV.xy * 0.5f + 0.5f; 
    lightSpaceUV.y = 1 - lightSpaceUV.y;
    float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, D).r;  
    
    return shadow*saturate(directLightDiffuse + directLightSpecular) * myDirectionalLight.Color * myDirectionalLight.Power * NdotL;
}


DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    //Buffer maps are now filled from gbuffer? not sent indivudually by the rendercommands??
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
     
    
    const float3 radiance =
    CalculateDirectionLight(diffuseColor, specularColor, Normal.xyz, cameraDirection, Material.g, worldPosition); 
    result.Color.rgb = (radiance + Effect.r) * albedo.rgb; 
    result.Color.rgb = saturate(LinearToGamma(result.Color.rgb));
    result.Color.a = 1.0f;
    return result;
}