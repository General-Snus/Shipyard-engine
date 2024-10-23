#include "../../Headers/ShaderStructs.hlsli"  

struct GBufferOutput
{
    float4 Albedo : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Material : SV_TARGET2;
    float4 Effect : SV_TARGET3;
    float4 VertexNormal : SV_TARGET4;
    float4 WorldPosition : SV_TARGET5; 
    float4 packedData : SV_TARGET6;   // xy = velocity, zw = id 
};

float4 PackData(float2 velocity, uint id )
{ 
    float packedID =  asfloat(id);
    return float4(velocity, packedID, 0);
}

void UnpackData(float4 packedData, out float2 velocity, out uint id, out float depth)
{
    velocity = packedData.xy;
    id = asuint(packedData.z); 
}

GBufferOutput main(DefaultVertexToPixel input)
{
	GBufferOutput result;
	const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.BiNormal),
		normalize(input.Normal)
	);

	const float3 cameraDirection = g_FrameBuffer.FB_CameraPosition.xyz - input.WorldPosition.xyz;
	const float2 uv = input.UV * g_defaultMaterial.UVTiling;

	const float4 textureColor = textureHeap[g_defaultMaterial.AlbedoTextureIndex].Sample(defaultSampler,uv) * g_defaultMaterial.albedoColor;

	if (textureColor.a < 0.1f)
	{
		discard;
	}

	const float4 materialComponent = textureHeap[g_defaultMaterial.MaterialTextureIndex].Sample(defaultSampler,uv);
	const float2 textureNormal = textureHeap[g_defaultMaterial.NormalTextureIndex].Sample(defaultSampler,uv).xy;
	const float4 effect = textureHeap[g_defaultMaterial.EmissiveTextureIndex].Sample(defaultSampler,uv);

	//Normals
	float3 pixelNormal;
	pixelNormal.xy = ((2.0f * textureNormal.xy) - 1.0f);
	pixelNormal.z = sqrt(1 - (pow(pixelNormal.x,2.0f) + pow(pixelNormal.y,2.0f)));
	pixelNormal = normalize(mul(pixelNormal,TBN));
	pixelNormal *= g_defaultMaterial.NormalStrength;

	result.Albedo = textureColor;

	result.Normal.xyz = pixelNormal;
	result.Normal.w = 1;

	result.Material = materialComponent;
	result.Material.g *= g_defaultMaterial.Roughness;

	result.Effect = effect;

	result.VertexNormal.xyz = input.Normal;	
	result.VertexNormal.w = 1;

	result.WorldPosition = input.WorldPosition;
	
	result.packedData = PackData(float2(0.1,0.1),g_ObjectBuffer.OB_ID); 

	return result;
}