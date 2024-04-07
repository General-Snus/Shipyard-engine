#include "../Registers.h" 
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
    result.Normal = input.Normal;
    result.Tangent = input.Tangent;
    result.BiNormal = cross(input.Normal, input.Tangent);
    
    //if(g_ObjectBuffer.hasBone)
    //{
    //    float4x4 skinMatrix = 0;
    //    skinMatrix += g_ObjectBuffer.OB_BoneTransform[input.BoneIds[0]] * input.BoneWeights[0];
    //    skinMatrix += g_ObjectBuffer.OB_BoneTransform[input.BoneIds[1]] * input.BoneWeights[1];
    //    skinMatrix += g_ObjectBuffer.OB_BoneTransform[input.BoneIds[2]] * input.BoneWeights[2];
    //    skinMatrix += g_ObjectBuffer.OB_BoneTransform[input.BoneIds[3]] * input.BoneWeights[3];
    //    result.Position = mul(skinMatrix, result.Position);
    //    
    //    const float3x3 skinNormalRotation = (float3x3)transpose(skinMatrix);
    //    result.Normal = mul(input.Normal, skinNormalRotation);
    //    result.BiNormal = mul(result.BiNormal, (float3x3)skinMatrix);
    //    result.Tangent = mul(result.Tangent, (float3x3)skinMatrix);
    //}
    
    
    
    float3x3 worldNormalRotation = (float3x3)g_ObjectBuffer.OB_Transform;
    result.WorldPosition = mul(g_ObjectBuffer.OB_Transform, result.Position);
    
    if (g_ObjectBuffer.OB_Instanced)
    {
    //    worldNormalRotation = (float3x3)input.World;
    //    result.WorldPosition = mul(input.World, result.Position);
    }
    
    
    result.Normal = mul(result.Normal, transpose(worldNormalRotation));
    result.BiNormal = mul(result.BiNormal, worldNormalRotation);
    result.Tangent = mul(result.Tangent, worldNormalRotation);
     
    result.Position = mul(g_FrameBuffer.FB_InvView, result.WorldPosition);
    result.Position = mul(g_FrameBuffer.FB_Proj, result.Position);
    //result.Position.z = 1 - clamp(result.Position.z, 0, 1);
   //
   //result.Position.z = log2(max(1e-6, 1.0 + result.Position.w)) * Fcoef - 1.0;
    
    float4 vertexColorBasedOnPositionInBox;
    vertexColorBasedOnPositionInBox.xy = input.UV.xy;
    vertexColorBasedOnPositionInBox.z = 1;
    vertexColorBasedOnPositionInBox.w = 1;
    result.VxColor = vertexColorBasedOnPositionInBox;
    return result;
}
  