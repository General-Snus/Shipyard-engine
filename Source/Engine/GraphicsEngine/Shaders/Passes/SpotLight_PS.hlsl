//#include "../Headers/PBRFunctions.hlsli" 

//float spotFallof(float spectatorAngle, float umbralAngle, float penumbralAngle)
//{
//     [flatten]
//    if(spectatorAngle > penumbralAngle)
//        return 1;
    
//     [flatten]
//    if(spectatorAngle < umbralAngle)
//        return 0;
    
//    const float denom = spectatorAngle - umbralAngle;
//    const float numer = (penumbralAngle - umbralAngle);
//    const float conalAtt = pow((denom / numer), 4);
//    return conalAtt;
//}

//float LinearizeDepth(float depth, const hlslSpotLight spotLight)
//{
    
//    float near_plane = .1f;
//    float far_plane = spotLight.Range;
    
//    float z = depth * 2.0 - 1.0; // Back to NDC 
//    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
//}

//float3 CalculateSpotLight(float3 diffuseColor, float3 specularColor, float4 worldPosition, float3 normal, float3 cameraDirection, float roughness, float shine, const hlslSpotLight spotLight)
//{
//    float3 RayFromSpot = (spotLight.Position - worldPosition.xyz);
//    const float dist = length(RayFromSpot);
//    RayFromSpot = normalize(RayFromSpot);
//    const float3 halfAngle = normalize(normalize(cameraDirection) + RayFromSpot);
//    const float3 spotlightDirection = normalize(spotLight.Direction);
    
//    const float spectatorAngle = dot(spotlightDirection, -RayFromSpot);
//    const float umbralAngle = cos(clamp(spotLight.OuterConeAngle / 2, 0.001f, PI));
//    const float penumbralAngle = cos(clamp(spotLight.InnerConeAngle / 2, 0.001f, PI - umbralAngle));
//    const float conalAtt = spotFallof(spectatorAngle, umbralAngle, penumbralAngle);
    
//  //  const float3 Attenuation = 1 - saturate(pow(dist * pow(max(spotLightData.Range, 0.000001f), -1), 2));
//    const float3 Attenuation = conalAtt / pow(max(dist, spotLight.Range), 2); //REFACTOR: PHYSICS i hate this

//    const float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, RayFromSpot, halfAngle, roughness);
//    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
//    directLightDiffuse *= (1.0f - directLightSpecular);
    
//    const float NdotL = saturate(dot(RayFromSpot, normal));
//    const float solidAngle = 2 * PI * (1 - .5f * (penumbralAngle + umbralAngle)); // Solid angle of a cone
//    const float radiantIntensity = spotLight.Power / solidAngle; // Arbitrary conversion from watts
    
    
//    const float4x4 lightView = spotLight.lightView;
//    const float4x4 lightProj = spotLight.projection;
    
//    float4 lightSpacePos = mul(lightView, worldPosition);
//    lightSpacePos = mul(lightProj, lightSpacePos);
    
//    float3 lightSpaceUV = 0;
//    lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
//    lightSpaceUV.y = 1 - ((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
//    const float bias = 0.0005;
//    float Depth = (lightSpacePos.z / lightSpacePos.w) + bias;
//    //float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, Depth).r; 
//    //Enable if quality is too low
//    uint2 dim = 0;
//    uint numMips = 0;
//    shadowMap.GetDimensions(0, dim.x, dim.y, numMips);
//    float2 texelSize = 1.0 / dim;
    
//    float sum = 0;
//    float x, y;
//    for(y = -1.5; y <= 1.5; y += 1.0)
//    {
//        for(x = -1.5; x <= 1.5; x += 1.0)
//        {
//            float2 newUV;
//            newUV.x = lightSpaceUV.x + x * texelSize.x;
//            newUV.y = lightSpaceUV.y + y * texelSize.y;
//            sum += shadowMap.SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
//        }
//    }
//    const float shadow = sum / 16.0;
    
    
//    return /*shadow * */saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * normalize(spotLight.Color);
//    //saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * spotLightData.Color;
//}

//DefaultPixelOutput main(BRDF_VS_to_PS input)
//{
//    DefaultPixelOutput result;
//    float2 uv = input.UV;
    
//    const float4 albedo = colorPass.Sample(defaultSampler, uv);
//    const float4 Material = normalPass.Sample(defaultSampler, uv);
//    const float4 Normal = materialPass.Sample(defaultSampler, uv);
//    const float4 Effect = effectPass.Sample(defaultSampler, uv);
//    const float4 vertexNormal = vertexNormalPass.Sample(defaultSampler, uv);
//    const float4 worldPosition = float4(normalize(worldPositionPass.Sample(defaultSampler, uv).xyz), 1);
//    const float metallic = Material.b;
//    const float roughness = Material.g;
//    const float occlusion = Material.r;
//    const float depth = DepthPass.Sample(defaultSampler, uv).r;
//    const float4 ssao = SSAOPass.Sample(defaultSampler, uv);
    
//    const float3 cameraDirection = normalize(g_FrameBuffer.FB_CameraPosition.xyz - worldPosition.xyz);
//    const float3 diffuseColor = lerp((float3)0.0f, albedo.rgb, 1 - metallic);
//    const float3 specularColor = lerp((float3)0.04f, albedo.rgb, metallic);
     
//    float3 totalSpotLightContribution = 0;

//    int i = 0;
//    for(i = 0; i < g_lightBuffer.spotLightAmount; i += 1)
//    {
//        const hlslSpotLight spotLight = g_lightBuffer.mySpotLight[i]; //
//        [flatten]
//        if(spotLight.Power > 0)
//        {
//            totalSpotLightContribution += CalculateSpotLight(
//            diffuseColor,
//            specularColor,
//            worldPosition,
//            Normal.xyz,
//            cameraDirection,
//            roughness,
//            g_defaultMaterial.Shine,
//            spotLight
//            );
//        }
//    }
   
//    const float3 radiance = totalSpotLightContribution;
//    result.Color.rgb = radiance;
//    result.Color.a = 1.0f;
    
//    return result;
//}