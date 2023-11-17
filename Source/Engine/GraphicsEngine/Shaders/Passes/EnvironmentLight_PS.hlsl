#include "../Headers/PBRFunctions.hlsli"  

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
    const float3 directionToLight = -myDirectionalLight.Direction.xyz;
    const float3 halfAngle = normalize(cameraDirection + directionToLight);
    const float NdotL = saturate(dot(normal, directionToLight));
    const float K = pow(roughness + 1, 2) / 8;

    float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, directionToLight, halfAngle, roughness);
    float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
    directLightDiffuse *= (1.0f - directLightSpecular);
    
    const float4x4 lightView = myDirectionalLight.lightView;
    const float4x4 lightProj = myDirectionalLight.projection;
    
    float4 lightSpacePos = mul(lightView, worldPosition);
    lightSpacePos = mul(lightProj, lightSpacePos);
    
    float3 lightSpaceUV = 0;
    lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
    lightSpaceUV.y = 1- ((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
     
    const float bias = max(0.05 * (1.0 - NdotL), 0.005);
    const float Depth = (lightSpacePos.z / lightSpacePos.w) - bias  ;
    //const float shadow =  shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy,Depth).r; 
    
    
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
            newUV.x = lightSpaceUV.x + x*texelSize.x;
            newUV.y = lightSpaceUV.y + y * texelSize.y;
            
            sum +=  shadowMap.SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
        }
    }
    const float shadow = sum / 16.0;
    
    const float ShadowStrength = 1;
    return ShadowStrength * shadow * saturate(directLightDiffuse + directLightSpecular) * myDirectionalLight.Color * myDirectionalLight.Power * NdotL;
}


DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    //Buffer maps are now filled from gbuffer? not sent indivudually by the rendercommands??
    DefaultPixelOutput result;
    const float2 uv = input.UV;
    
    const float4 albedo = colorMap.Sample(defaultSampler, uv);
    const float4 Material = materialMap.Sample(defaultSampler, uv);
    const float4 Normal = normalMap.Sample(defaultSampler, uv);
    const float4 Effect = effectMap.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(worldPositionMap.Sample(defaultSampler, uv).xyz, 1);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r * SSAOMap.Sample(defaultSampler, uv).r;
    
    const float3 cameraDirection = normalize(FB_CameraPosition.xyz - worldPosition.xyz);
    const float3 diffuseColor = lerp((float3)0.0f, albedo.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3)0.04f, albedo.rgb, metallic); 
    
    const float3 radiance =
    CalculateDirectionLight(diffuseColor, specularColor, Normal.xyz, cameraDirection, roughness, worldPosition)
    + CalculateIndirectLight(diffuseColor, specularColor, Normal.xyz, cameraDirection, enviromentCube, roughness, occlusion);
    
    result.Color.rgb =  radiance + Effect.r * albedo.rgb; 
    result.Color.a = 1.0f;
    return result;
}