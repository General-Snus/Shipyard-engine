 
#include "../../../Headers/PBRFunctions.hlsli"
#include "../../../Registers.h" 
 

float3 CalculatePointLight(float3 diffuseColor, float3 specularColor, float4 worldPosition, float3 normal, float3 cameraDirection, hlslPointLight pointLightData, float roughness, float shine)
{
    float3 lightDirection = (pointLightData.Position - worldPosition.xyz);
    const float distance = length(lightDirection);
    
    lightDirection = normalize(lightDirection);
    const float3 halfAngle = normalize(cameraDirection + lightDirection);
    
    const float constantAtt = 1.0f;
    const float linearAtt = 4.6 * pow(distance, -1);
    const float quadraticAtt = 8.9 * pow(distance, -0.5);
    
    //const float3 Attenuation = 1 / (constantAtt + distance * (linearAtt + quadraticAtt * distance));
    const float3 Attenuation = 1 / pow(max(distance, pointLightData.Range), 2);
    
    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, lightDirection, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    const float NdotL = dot(lightDirection, normal);
    
    //const float4x4 lightView = pointLightData.lightView;
    //const float4x4 lightProj = pointLightData.projection;
    
    //float4 lightSpacePos = mul(lightView, worldPosition);
    //lightSpacePos = mul(lightProj, lightSpacePos);
    
    //float3 lightSpaceUV = 0;
    //lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
    //lightSpaceUV.y = 1 - ((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
    //const float bias = 0.0005;
    //float Depth = (lightSpacePos.z / lightSpacePos.w) + bias;
    ////float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, Depth).r;
     
    ////Enable if quality is too low
    //uint2 dim = 0;
    //uint numMips = 0;
    //shadowMap.GetDimensions(0, dim.x, dim.y, numMips);
    //float2 texelSize = 1.0 / dim;
    
    //float sum = 0;
    //float x, y;
    //for (y = -1.5; y <= 1.5; y += 1.0)
    //{
    //    for (x = -1.5; x <= 1.5; x += 1.0)
    //    {
    //        float2 newUV;
    //        newUV.x = lightSpaceUV.x + x * texelSize.x;
    //        newUV.y = lightSpaceUV.y + y * texelSize.y;
    //        sum += shadowMap.SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
    //    }
    //}
    //const float shadow = sum / 16.0;
    
    
    //return shadow * Attenuation;  
    return /*shadow **/ saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * pointLightData.Color * pointLightData.Power;
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
    
    const float3 cameraDirection = normalize(g_FrameBuffer.FB_CameraPosition.xyz - worldPosition.xyz);
    const float3 diffuseColor = lerp((float3) 0.0f, albedo.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metallic);
    
    float3 totalPointLightContribution = 0;
    int i = 0;
    for (i = 0; i < g_lightBuffer.pointLightAmount; i += 1)
    {
	    const hlslPointLight pointLight = g_lightBuffer.myPointLight[i]; //
        [flatten]
        if (pointLight.Power > 0)
        {
            totalPointLightContribution += CalculatePointLight(
            diffuseColor,
            specularColor,
            worldPosition,
            Normal.xyz,
            cameraDirection,
            pointLight,
            roughness,
            g_defaultMaterial.DefaultMaterial.Shine            
            );
        }
    }
   
    const float3 radiance = totalPointLightContribution;
    result.Color.rgb = radiance;
    result.Color.a = 1.0f;
    
    return result;
}