#include "../Headers/SceneGraph.hlsli"  
#include "../Headers/PBRFunctions.hlsli"



DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    DefaultPixelOutput result;
    const float2 uv = input.UV;

    const float4 albedo = colorPass.Sample(defaultSampler, uv);
    const float4 Normal = normalPass.Sample(defaultSampler, uv);
    const float4 Material = materialPass.Sample(defaultSampler, uv);
    const float4 Effect = effectPass.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(worldPositionPass.Sample(defaultSampler, uv).xyz, 1);

    const float occlusion = Material.r * 1 /*SSAOMap.Sample(defaultSampler, uv).r*/;
    const float roughness = Material.g;
    const float metallic = Material.b;

    const float3 cameraDirection = normalize(g_FrameBuffer.FB_CameraPosition.xyz - worldPosition.xyz);
    const float3 diffuseColor = lerp((float3) 0.0f, albedo.rgb, 1 - metallic);
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metallic);

    const float3 enviromentalLight = CalculateIndirectLight(diffuseColor, specularColor, Normal.xyz, cameraDirection, environmentCube, roughness, occlusion);
    const float3 directLight = CalculateDirectionLight(diffuseColor, specularColor, Normal.xyz, cameraDirection, roughness, worldPosition);
    const float3 ambientLight = albedo.rgb * .1f;
    int i = 0;
    float3 totalSpotLightContribution = 0;
    for (i = 0; i < 8; i += 1)
    {
        const hlslSpotLight spotLight = g_lightBuffer.mySpotLight[i]; // 
        if (spotLight.Power > 0)
        {
            totalSpotLightContribution += CalculateSpotLight(
	 				diffuseColor,
	 				specularColor,
	 				worldPosition,
	 				Normal.xyz,
	 				cameraDirection,
	 				roughness,
	 				g_defaultMaterial.Shine,
	 				spotLight
	 			);
        }
    }
	 
	 
    float3 totalPointLightContribution = 0; 
    for (i = 0; i < 8; i += 1)
    {
        const hlslPointLight pointLight = g_lightBuffer.myPointLight[i]; // 
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
					g_defaultMaterial.Shine
				);
        }
	
    }
	


    const float3 radiance = directLight + enviromentalLight + totalPointLightContribution + totalSpotLightContribution;

    result.Color.rgb = radiance + Effect.r * albedo.rgb;
    result.Color.a = 1.0f;
    return result;
}