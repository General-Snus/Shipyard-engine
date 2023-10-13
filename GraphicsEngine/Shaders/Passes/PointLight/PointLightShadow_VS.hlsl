#include "../../Headers/ShaderStructs.hlsli"



VertexOutput main(
float4 Position : POSITION,
uint4 BoneIds : BONEIDS,
float4 BoneWeights : BONEWEIGHTS)
{
    VertexOutput output;
    output.Position = Position;
    
    if(hasBone)
    {
        float4x4 skinMatrix = 0;
        skinMatrix += OB_BoneTransform[BoneIds[0]] * BoneWeights[0];
        skinMatrix += OB_BoneTransform[BoneIds[1]] * BoneWeights[1];
        skinMatrix += OB_BoneTransform[BoneIds[2]] * BoneWeights[2];
        skinMatrix += OB_BoneTransform[BoneIds[3]] * BoneWeights[3];
        output.Position = mul(skinMatrix, output.Position);
    }
    
    output.Position = mul(OB_Transform, Position);
    
    return output;
}