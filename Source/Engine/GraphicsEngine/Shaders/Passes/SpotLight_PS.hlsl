#include "../Headers/ShaderStructs.hlsli"
#include "../Headers/PBRFunctions.hlsli" 

float spotFallof(float spectatorAngle, float umbralAngle, float penumbralAngle)
{
     [flatten]
    if(spectatorAngle > penumbralAngle)
        return 1;
    
     [flatten]
    if(spectatorAngle < umbralAngle)
        return 0;
    
    const float denom = spectatorAngle - umbralAngle;
    const float numer = (penumbralAngle - umbralAngle);
    const float conalAtt = pow((denom / numer), 4);
    return conalAtt;
}

float LinearizeDepth(float depth)
{
    
    float near_plane = .1f;
    float far_plane = mySpotLight.Range;
    
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float3 CalculateSpotLight(float3 diffuseColor, float3 specularColor, float4 worldPosition, float3 normal, float3 cameraDirection, float roughness, float shine)
{
    float3 RayFromSpot = (mySpotLight.Position - worldPosition.xyz);
    const float dist = length(RayFromSpot);
    RayFromSpot = normalize(RayFromSpot);
    const float3 halfAngle = normalize(normalize(cameraDirection) + RayFromSpot);
    const float3 spotlightDirection = normalize(mySpotLight.Direction);
    
    const float spectatorAngle = dot(spotlightDirection, -RayFromSpot);
    const float umbralAngle = cos(clamp(mySpotLight.OuterConeAngle / 2, 0.001f, PI));
    const float penumbralAngle = cos(clamp(mySpotLight.InnerConeAngle / 2, 0.001f, PI - umbralAngle));
    const float conalAtt = spotFallof(spectatorAngle, umbralAngle, penumbralAngle);
    
  //  const float3 Attenuation = 1 - saturate(pow(dist * pow(max(spotLightData.Range, 0.000001f), -1), 2));
    const float3 Attenuation = conalAtt / pow(max(dist, mySpotLight.Range), 2); //REFACTOR: PHYSICS i hate this
      
    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, RayFromSpot, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    const float NdotL = saturate(dot(RayFromSpot, normal));
    const float solidAngle = 2 * PI * (1 - .5f * (penumbralAngle + umbralAngle)); // Solid angle of a cone
    const float radiantIntensity = mySpotLight.Power / solidAngle; // Arbitrary conversion from watts
    
    
    const float4x4 lightView = mySpotLight.lightView;
    const float4x4 lightProj = mySpotLight.projection;
    
    float4 lightSpacePos = mul(lightView, worldPosition);
    lightSpacePos = mul(lightProj, lightSpacePos);
    
    float3 lightSpaceUV = 0;
    lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
    lightSpaceUV.y = 1-((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
    const float bias = 0.0005;
    float Depth = (lightSpacePos.z / lightSpacePos.w) - bias;
    float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, Depth).r;
     
    //Enable if quality is too low
    uint2 dim = 0;
    uint numMips = 0;
    shadowMap.GetDimensions(0, dim.x, dim.y, numMips);
    float2 texelSize = 1.0 / dim;
    
    float sum = 0;
    float x, y;
    for(y = -1.5; y <= 1.5; y += 1.0)
    {
        for(x = -1.5; x <= 1.5; x += 1.0)
        {
            float2 newUV;
            newUV.x = lightSpaceUV.x + x * texelSize.x;
            newUV.y = lightSpaceUV.y + y * texelSize.y; 
            sum += shadowMap.SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
        }
    }
    shadow = sum / 16.0;
    
    
    return shadow * saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * normalize(mySpotLight.Color);
    //saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * spotLightData.Color;
}

DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    DefaultPixelOutput result;
    float2 uv = input.UV;
    
    const float4 albedo = colorMap.Sample(defaultSampler, uv);
    const float4 Material = materialMap.Sample(defaultSampler, uv);
    const float4 Normal = normalMap.Sample(defaultSampler, uv);
    const float4 Effect = effectMap.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(worldPositionMap.Sample(defaultSampler, uv).xyz, 1);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r;
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - worldPosition.xyz);
    const float3 diffuseColor = lerp((float3)0.0f, albedo.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3)0.04f, albedo.rgb, metallic);
     
    float3 totalSpotLightContribution = 0;
        [flatten]
    if(mySpotLight.Power > 0)
    {
        totalSpotLightContribution += CalculateSpotLight(
            diffuseColor,
            specularColor,
            worldPosition,
            Normal.xyz,
            cameraDirection,
            roughness,
            DefaultMaterial.Shine            
            );
    }
   
    const float3 radiance = totalSpotLightContribution;
    result.Color.rgb = radiance;
    result.Color.a = 1.0f;
    
    return result;
}