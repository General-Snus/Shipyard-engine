#include "../Registers.h" 
#include "../Headers/ShaderStructs.hlsli"  
#include "../Headers/Common.hlsli"  
 
float2 GetRandom(float2 uv, float2 uvScale)
{
	const float3 random = 2.0f * Noise_Texture.Sample(PointSampler, uv * uvScale).rgb - 1.0f;
	return random.xy;
}

float4 GetViewPosition(float2 uv)
{
	const float4 worldPosition = float4(worldPositionPass.Sample(defaultSampler, uv).xyz, 1);
	const float4 viewPosition = mul(g_FrameBuffer.FB_InvView, worldPosition);
	return viewPosition;
}

float4 GetViewNormal(float2 uv)
{
	const float4 worldNormal = float4(textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler, uv).xyz, 0);
	const float4 viewNormal = mul(g_FrameBuffer.FB_InvView, worldNormal);
	return viewNormal;
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
	const float2 uv = float2(NdotV, roughness);

	const float2 brdfLUT = BRDF_LUT_Texture.Sample(BRDFSampler, uv).rg;
	
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

float LinearizeDepth(float depth, const hlslSpotLight spotLight)
{
	const float near_plane = .1f;
	const float far_plane = spotLight.Range;

	const float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float3 CalculateSpotLight(float3 diffuseColor, float3 specularColor, float4 worldPosition, float3 normal, float3 cameraDirection, float roughness, float shine, const hlslSpotLight spotLight)
{
	float3 RayFromSpot = (spotLight.Position - worldPosition.xyz);
	const float dist = length(RayFromSpot);
	RayFromSpot = normalize(RayFromSpot);
	const float3 halfAngle = normalize(normalize(cameraDirection) + RayFromSpot);
	const float3 spotlightDirection = normalize(spotLight.Direction);
	
	const float spectatorAngle = dot(spotlightDirection, -RayFromSpot);
	const float umbralAngle = cos(clamp(radians(spotLight.OuterConeAngle) / 2, 0.001f, PI));
	const float penumbralAngle = cos(clamp(radians(spotLight.InnerConeAngle) / 2, 0.001f, PI - umbralAngle));
	const float conalAtt = spotFallof(spectatorAngle, umbralAngle, penumbralAngle);
	
  //  const float3 Attenuation = 1 - saturate(pow(dist * pow(max(spotLightData.Range, 0.000001f), -1), 2));
	const float3 Attenuation = conalAtt / pow(max(dist, spotLight.Range), 2); //REFACTOR: PHYSICS i hate this

	const float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, RayFromSpot, halfAngle, roughness);
	float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
	directLightDiffuse *= (1.0f - directLightSpecular);
	
	const float NdotL = saturate(dot(RayFromSpot, normal));
	const float solidAngle = 2 * PI * (1 - .5f * (penumbralAngle + umbralAngle)); // Solid angle of a cone
	const float radiantIntensity = spotLight.Power / solidAngle; // Arbitrary conversion from watts
	float shadow = 1.0f;
	
	
	if (spotLight.castShadow)
	{
		const float4x4 lightView = spotLight.lightView;
		const float4x4 lightProj = spotLight.projection;
	
		float4 lightSpacePos = mul(lightView, worldPosition);
		lightSpacePos = mul(lightProj, lightSpacePos);
	
		float3 lightSpaceUV = 0;
		lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
		lightSpaceUV.y = 1 - ((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
		const float bias = 0.0005;
		const float Depth = (lightSpacePos.z / lightSpacePos.w) + bias;
	//float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, Depth).r; 
	//Enable if quality is too low
		uint2 dim = 0;
		uint numMips = 0;
		textureHeap[spotLight.shadowMapIndex].GetDimensions(0, dim.x, dim.y, numMips);
		float2 texelSize = 1.0 / dim;
	
		float sum = 0;
		float x, y;
		for (y = -1.5; y <= 1.5; y += 1.0)
		{
			for (x = -1.5; x <= 1.5; x += 1.0)
			{
				float2 newUV;
				newUV.x = lightSpaceUV.x + x * texelSize.x;
				newUV.y = lightSpaceUV.y + y * texelSize.y;
				sum += textureHeap[spotLight.shadowMapIndex].SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
			}
		}
		shadow = sum / 16.0;
	}
	
	
	return shadow * saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * normalize(spotLight.Color);
	//saturate(directLightDiffuse + directLightSpecular) * NdotL * Attenuation * radiantIntensity * spotLightData.Color;
}


float3 CalculatePointLight(float3 diffuseColor, float3 specularColor, float4 worldPosition, float3 normal, float3 cameraDirection, hlslPointLight pointLightData, float roughness, float shine)
{
	float3 lightDirection = (pointLightData.Position - worldPosition.xyz);
	const float distance = length(lightDirection);
	
	lightDirection = normalize(lightDirection);
	const float3 halfAngle = normalize(cameraDirection + lightDirection);
	
	const float range = max(pointLightData.Range, 1.f);
	const float InnerStrength = pow(distance / range, 4);
	const float falloff = pow(saturate(1 - InnerStrength), 2) / (pow(distance, 2) + 1);
	
	//const float3 Attenuation = 1 / (constantAtt + distance * (linearAtt + quadraticAtt * distance));
	//const float3 Attenuation = 1 / pow(max(distance, pointLightData.Range), 2);

	const float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, lightDirection, halfAngle, roughness);
	float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
	directLightDiffuse *= (1.0f - directLightSpecular);
	
	const float NdotL = max(0.0, dot(lightDirection, normal));
	
	float shadow = 1.0f;
	if (pointLightData.castShadow)
	{
		uint2 dim = 0;
		uint numMips = 0;
		textureHeap[pointLightData.shadowMapIndex[0]].GetDimensions(0, dim.x, dim.y, numMips);
		const float2 texelSize = 1.0 / dim;
		
		shadow = 0.f;
		
	[unroll]
		for (int shadowmapIndex = 0; shadowmapIndex < 6; shadowmapIndex += 1)
		{
	
			const float4x4 lightView = pointLightData.lightView[shadowmapIndex];
			float4 lightSpacePos = mul(lightView, worldPosition);
			const float4x4 lightProj = pointLightData.projection;
			lightSpacePos = mul(lightProj, lightSpacePos);
	
			float3 lightSpaceUV = 0;
			lightSpaceUV.x = ((lightSpacePos.x / lightSpacePos.w) * .5 + 0.5);
			lightSpaceUV.y = 1 - ((lightSpacePos.y / lightSpacePos.w) * .5 + 0.5f);
			const float bias = 0.0005;
			const float Depth = (lightSpacePos.z / lightSpacePos.w) + bias;
		 
			
			const int heapIndex = pointLightData.shadowMapIndex[shadowmapIndex];
			
			//If you want better quality go this
			//float sum = 0;
			//float x, y;
			//for (y = -1.5; y <= 1.5; y += 1.0)
			//{
			//    for (x = -1.5; x <= 1.5; x += 1.0)
			//    {
			//        float2 newUV;
			//        newUV.x = lightSpaceUV.x + x * texelSize.x;
			//        newUV.y = lightSpaceUV.y + y * texelSize.y;
			//        sum += textureHeap[heapIndex].SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
			//    }
			//}
			//shadow += sum / 16.0;
			
			shadow += textureHeap[heapIndex].SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, Depth).r;
		}
	}
	return shadow * saturate(directLightDiffuse + directLightSpecular) * NdotL * falloff * pointLightData.Color * pointLightData.Power;
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
	const float3 directionToLight = -g_lightBuffer.myDirectionalLight.Direction.xyz;
	const float3 halfAngle = normalize(cameraDirection + directionToLight);
	const float NdotL = saturate(dot(normal, directionToLight));

	const float3 directLightSpecular = CalculateSpecularLight(specularColor, normal, cameraDirection, directionToLight, halfAngle, roughness);
	float3 directLightDiffuse = CalculateDiffuseLight(diffuseColor);
	directLightDiffuse *= (1.0f - directLightSpecular);
	float shadow = 1.f;
	
	if (g_lightBuffer.myDirectionalLight.castShadow)
	{
		shadow = 0.f;
		const float4x4 lightView = g_lightBuffer.myDirectionalLight.lightView;
		const float4x4 lightProj = g_lightBuffer.myDirectionalLight.projection;
	
		float4 lightSpacePos = mul(lightView, worldPosition);
		lightSpacePos = mul(lightProj, lightSpacePos);

		float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
		lightSpaceUV.x = (lightSpacePos.x * .5 + 0.5);
		lightSpaceUV.y = 1 - (lightSpacePos.y * .5 + 0.5f);
	 
		const float bias = 0.005f;
		const float Depth = lightSpaceUV.z + bias;
	
		uint2 dim = 0;
		uint numMips = 0;
		textureHeap[g_lightBuffer.myDirectionalLight.shadowMapIndex].GetDimensions(0, dim.x, dim.y, numMips);
		float2 texelSize = 1.0 / dim;
   
		float sum = 0;
		for (float y = -1.5; y <= 1.5; y += 1.0)
		{
			for (float x = -1.5; x <= 1.5; x += 1.0)
			{
				float2 newUV;
				newUV.x = lightSpaceUV.x + x * texelSize.x;
				newUV.y = lightSpaceUV.y + y * texelSize.y;
		  
				sum += textureHeap[g_lightBuffer.myDirectionalLight.shadowMapIndex].SampleCmpLevelZero(shadowCmpSampler, newUV, Depth).r;
			}
		}
		shadow = sum / 16.0;
	}
	 
	return shadow * saturate(directLightDiffuse + directLightSpecular) * g_lightBuffer.myDirectionalLight.Color * g_lightBuffer.myDirectionalLight.Power * NdotL;
}
