#include "../Headers/ShaderStructs.hlsli" 

float3 PositionInBound(float3 aMin, float3 aMax, float3 aPosition)
{
    const float3 boundSize = aMax - aMin;
    return aPosition / boundSize;
}

DefaultVertexToPixel main(DefaultVertexInput input)
{
    DefaultVertexToPixel result;
    result.UV = input.UV;
    result.Position = input.Position;
    result.WorldPosition = input.Position;
    
    result.Normal = input.Normal;
    result.Tangent = input.Tangent;
    result.BiNormal = cross(input.Normal, input.Tangent);
    
    if (hasBone)
    {
        float4x4 skinMatrix = 0;
        skinMatrix += OB_BoneTransform[input.BoneIds[0]] * input.BoneWeights[0];
        skinMatrix += OB_BoneTransform[input.BoneIds[1]] * input.BoneWeights[1];
        skinMatrix += OB_BoneTransform[input.BoneIds[2]] * input.BoneWeights[2];
        skinMatrix += OB_BoneTransform[input.BoneIds[3]] * input.BoneWeights[3];
        result.Position = mul(skinMatrix, result.Position);
        
        const float3x3 skinNormalRotation = (float3x3) transpose(skinMatrix);
        result.Normal = mul(input.Normal, skinNormalRotation);
        result.BiNormal = mul(result.BiNormal, (float3x3) skinMatrix);
        result.Tangent = mul(result.Tangent, (float3x3) skinMatrix);
    }
    
    const float3x3 worldNormalRotation = (float3x3) OB_Transform;
    result.Normal = mul(result.Normal, transpose(worldNormalRotation));
    result.BiNormal = mul(result.BiNormal, worldNormalRotation);
    result.Tangent = mul(result.Tangent, worldNormalRotation);
    
    
     //Local to world    
    result.Position = mul(OB_Transform, result.Position);
    result.WorldPosition = result.Position;
    result.Position = mul(FB_InvView, result.Position);
    result.Position = mul(FB_Proj, result.Position);
    
    float4 vertexColorBasedOnPositionInBox;
    vertexColorBasedOnPositionInBox.xy = input.UV.xy;
    vertexColorBasedOnPositionInBox.z = 1;
    vertexColorBasedOnPositionInBox.w = 1;
    result.VxColor = vertexColorBasedOnPositionInBox; 
    return result;
}
  