 
//float Sine(float aValue)
//{
//    const float pi2 = 6.28318548f;
//    const float shiftedValue = aValue * pi2;
//    return sin(shiftedValue);
//}

//float3 PositionInBound(float3 aMin, float3 aMax, float3 aPosition)
//{
//    const float3 boundSize = aMax - aMin;
//    return aPosition / boundSize;
//}
//int GetNumMips(TextureCube mipMap)
//{
//    int iWidth = 0;
//    int iHeight = 0;
//    int iNumMips = 0;
//    mipMap.GetDimensions(0, iWidth, iHeight, iNumMips);
//    return iNumMips;
//}

//float3 LinearToGamma(float3 aColor)
//{
//    return pow(abs(aColor), 1.0f / 2.2f);
//}

//float3 CalculateDiffuseIBL(float3 normal, TextureCube enviromentCube)
//{
//    const int CubeMips = GetNumMips(enviromentCube) - 1;
//    const float3 cubeMap = enviromentCube.SampleLevel(defaultSampler, normal, CubeMips).rgb;
    
//    return cubeMap;
//}
//float3 CalculateSpecularIBL(float3 specularColor, float3 normal, float3 cameraDirection, float roughness, TextureCube enviromentCube)
//{
//    const int CubeMips = GetNumMips(enviromentCube) - 1;
//    const float3 ReflectionVector = reflect(-cameraDirection, normal);
//    const float3 cubeMap = enviromentCube.SampleLevel(defaultSampler, ReflectionVector, roughness * CubeMips).rgb;
    
//    const float NdotV = saturate(dot(normal, cameraDirection));
//    const float2 brdfLUT = BRDF_LUT_Texture.Sample(BRDFSampler, float2(NdotV, roughness)).xy;
    
//    return cubeMap * (specularColor * brdfLUT.x + brdfLUT.y);;
//}

//float3 CalculateDiffuseLight(float3 diffuseColor)
//{
//    return (diffuseColor / PI);

//}
//float3 CalculateSpecularLight(float3 specularColor, float3 normal, float3 cameraDirection, float3 lightDirection, float3 halfAngle, float roughness)
//{
//    const float3 G = GeometricAttenuation_Smith_IBL(normal, cameraDirection, lightDirection, roughness);
     
//    const float3 D = pow(roughness, 4) / (PI * pow((pow(dot(normal, halfAngle), 2) * (pow(roughness, 4) - 1) + 1), 2));
    
//    const float fresnel = ((-5.55373 * saturate(dot(cameraDirection, halfAngle)) - 6.981316) * saturate(dot(cameraDirection, halfAngle)));
//    const float3 F = specularColor + (1 - specularColor) * pow(2, fresnel);
//    const float3 B = 4 * saturate(dot(normal, lightDirection)) * saturate(dot(normal, cameraDirection));
    
//    const float3 directLightSpecular = (D * F * G) / B;
    
//    return directLightSpecular;
//}


//float3 CalculateIndirectLight(
//float3 diffuseColor,
//float3 specularColor,
//float3 normal,
//float3 cameraDirection,
//TextureCube enviromentCube,
//float roughness,
//float occulusion
//)
//{
//    const float3 diffuse = CalculateDiffuseIBL(normal, enviromentCube);
//    const float3 specular = CalculateSpecularIBL(specularColor, normal, cameraDirection, roughness, enviromentCube);
    
//    const float3 kA = (diffuseColor * diffuse + specular) * occulusion * .3f;
    
//    return kA;
//}


//float3 CalculateDirectionLight(
//float3 diffuseColor,
//float3 specularColor,
//float3 normal,
//float3 cameraDirection,
//float roughness
//)
//{
//    const float3 directionToLight = -myDirectionalLight.Direction;
//    const float3 halfAngle = normalize(cameraDirection + directionToLight);
//    const float NdotL = saturate(dot(normal, directionToLight));
//    const float K = pow(roughness + 1, 2) / 8;

//    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, directionToLight, halfAngle, roughness);
//    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
//    directLightDiffuse *= (1.0f - directLightSpecular);
    
//    return saturate(directLightDiffuse + directLightSpecular) * myDirectionalLight.Color * myDirectionalLight.Power * NdotL;
//}

//float spotFallof(float spectatorAngle, float umbralAngle, float penumbralAngle)
//{
//     [flatten]
//    if (spectatorAngle > penumbralAngle)
//        return 1;
    
//     [flatten]
//    if (spectatorAngle < umbralAngle)
//        return 0;
    
//    const float denom = spectatorAngle - umbralAngle;
//    const float numer = (penumbralAngle - umbralAngle);
//    const float conalAtt = pow((denom / numer), 4);
//    return conalAtt;
//}

//float3 CalculateSpotLight(float3 diffuseColor, float3 specularColor, float3 worldPosition, float3 normal, float3 cameraDirection, SpotLight spotLightData, float roughness, float shine)
//{
//    float3 RayFromSpot = (spotLightData.Position - worldPosition);
//    const float dist = length(RayFromSpot);
//    RayFromSpot = normalize(RayFromSpot);
//    const float3 halfAngle = normalize(normalize(cameraDirection) + RayFromSpot);
//    const float3 spotlightDirection = normalize(spotLightData.Direction);
    
//    const float spectatorAngle = dot(spotlightDirection, -RayFromSpot);
//    const float umbralAngle = cos(clamp(spotLightData.OuterConeAngle / 2, 0.001f, PI));
//    const float penumbralAngle = cos(clamp(spotLightData.InnerConeAngle / 2, 0.001f, PI - umbralAngle));
//    const float conalAtt = spotFallof(spectatorAngle, umbralAngle, penumbralAngle);
    
//    //const float3 Attenuation = 1 - saturate(pow(dist * pow(max(spotLightData.Range, 0.000001f), -1), 2));
//    const float3 Attenuation = conalAtt / pow(max(dist, spotLightData.Range), 2); //REFACTOR: PHYSICS i hate this
      
//    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, RayFromSpot, halfAngle, roughness);
//    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
//    directLightDiffuse *= (1.0f - directLightSpecular);
    
//    const float NdotL = saturate(dot(RayFromSpot, normal));
//    const float solidAngle = 2 * PI * (1 - .5f * (penumbralAngle + umbralAngle)); // Solid angle of a cone
//    const float radiantIntensity = spotLightData.Power / solidAngle; // Arbitrary conversion from watts
//    return saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * spotLightData.Color;
//}

//float3 CalculatePointLight(float3 diffuseColor, float3 specularColor, float3 worldPosition, float3 normal, float3 cameraDirection, PointLight pointLightData, float roughness, float shine)
//{
//    float3 lightDirection = (pointLightData.Position - worldPosition);
//    const float distance = length(lightDirection);
    
//    lightDirection = normalize(lightDirection);
//    const float3 halfAngle = normalize(cameraDirection + lightDirection);
    
//    const float constantAtt = 1.0f;
//    const float linearAtt = 4.6 * pow(distance, -1);
//    const float quadraticAtt = 8.9 * pow(distance, -0.5);
    
//    const float3 Attenuation = 1 / (constantAtt + distance * (linearAtt + quadraticAtt * distance));
    
//    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, lightDirection, halfAngle, roughness);
//    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
//    directLightDiffuse *= (1.0f - directLightSpecular);
    
//    const float NdotL = dot(lightDirection, normal);
    
//    return saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * pointLightData.Color * pointLightData.Power;
//}

//DefaultPixelOutput main(DefaultVertexToPixel input)
//{
//    DefaultPixelOutput result;

//    const float3x3 TBN = float3x3(
//    normalize(input.Tangent),
//    normalize(input.BiNormal),
//    normalize(input.Normal)
//    );
    
//    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - input.WorldPosition.xyz);
//   // result.Color.rgb = (input.Normal.rgb + 1) / 2.0f;
//    const float2 uv = input.UV * DefaultMaterial.UVTiling;
    
//    const float4 textureColor = colorMap.Sample(defaultSampler, uv) * DefaultMaterial.albedoColor;
//    const float4 materialComponent = materialMap.Sample(defaultSampler, uv);
//    const float3 textureNormal = normalMap.Sample(defaultSampler, uv).xyz;
//    const float emmision = effectMap.Sample(defaultSampler, uv).r;
    
//    const float occlusion = materialComponent.r;
//    const float roughness = materialComponent.g;
//    const float metallic = materialComponent.b;
    
    
//    //Normals
//    float3 pixelNormal;
//    pixelNormal.xy = ((2.0f * textureNormal.xy) - 1.0f);
//    pixelNormal.z = sqrt(1 - (pow(pixelNormal.x, 2.0f) + pow(pixelNormal.y, 2.0f)));
//    pixelNormal = normalize(mul(pixelNormal, TBN));
//    pixelNormal *= DefaultMaterial.NormalStrength;
   
//    const float3 diffuseColor = lerp((float3) 0.0f, textureColor.rgb, 1 - metallic);
//    const float3 specularColor = lerp((float3) 0.04f, textureColor.rgb, metallic);
    
//    float3 totalPointLightContribution = 0;
//    float3 totalSpotLightContribution = 0;
    
//    [unroll]
//    for (uint p = 0; p < PointLightCount; p++)
//    {
//        [flatten]
//        if (myPointLight[p].Power > 0)
//        {
//            totalPointLightContribution += CalculatePointLight(
//            diffuseColor,
//            specularColor,
//            input.WorldPosition.xyz,
//            pixelNormal,
//            cameraDirection,
//            myPointLight[p],
//            roughness,
//            DefaultMaterial.Shine            
//            );
//        }
//    }
    
//    [unroll]
//    for (uint s = 0; s < SpotLightCount - 1; s++)
//    {
//        [flatten]
//        if (mySpotLight[s].Power > 0)
//        {
//            totalSpotLightContribution += CalculateSpotLight(
//            diffuseColor,
//            specularColor,
//            input.WorldPosition.xyz,
//            pixelNormal,
//            cameraDirection,
//            mySpotLight[s],
//            roughness,
//            DefaultMaterial.Shine            
//            );
//        }
//    }
    
   
//    const float3 radiance = totalSpotLightContribution + totalPointLightContribution +
//    CalculateDirectionLight(diffuseColor, specularColor, pixelNormal.xyz, cameraDirection, roughness)
//    + CalculateIndirectLight(diffuseColor, specularColor, pixelNormal.xyz, cameraDirection, enviromentCube, roughness, occlusion);
    
//    result.Color.rgb = (radiance + emmision) * textureColor.rgb;
  
    
//    result.Color.rgb = saturate(LinearToGamma(result.Color.rgb));
//    result.Color.a = 1.0f;
    
    
    
//#if _DEBUG
//    switch(FB_RenderMode)
//    {
//    default:
//    case 0:
//    {
//    break;
//    }
    
//    case 1:
//    {
//    result.Color.rgb = input.WorldPosition.xyz;
//    result.Color.a = 1.0f;
//    break;
//    }
    
//     case 2:
//    {
//    result.Color.rgb = input.VxColor;
//    result.Color.a = 1.0f;
//    break;
//    }
    
//    case 3:
//    {
//    result.Color.rgb = input.Normal;
//    result.Color.a = 1.0f;
//    break;
//    }
    
//    case 4:
//    {
//    result.Color.rgb = input.Tangent;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
//    case 5:
//    {
//    result.Color.rgb = input.BiNormal;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
//    case 6:
//    {
//    result.Color.rgb = textureColor.rgb;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
//     case 7:
//    {
//    result.Color.rgb = textureNormal.rgb;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
    
//     case 8:
//    {
//    result.Color.rgb = pixelNormal.rgb;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
//     case 9:
//    {
//    result.Color.rgb = occlusion;
//    result.Color.a = 1.0f;
//    break;
//    } 
//     case 10:
//    {
//    result.Color.rgb = roughness;
//    result.Color.a = 1.0f;
//    break;
//    } 
//     case 11:
//    {
//    result.Color.rgb = metallic;
//    result.Color.a = 1.0f;
//    break;
//    } 
//     case 12:
//    {
//    result.Color.rgb = emmision;
//    result.Color.a = 1.0f;
//    break;
//    } 
//     case 13:
//    {
//    result.Color.rgb = emmision;
//    result.Color.a = 1.0f;
//    break;
//    } 
    
//} 
//#endif
    
    
//    return result;
//} 