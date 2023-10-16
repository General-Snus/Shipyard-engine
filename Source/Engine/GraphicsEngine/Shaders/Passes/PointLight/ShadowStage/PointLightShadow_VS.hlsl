#include "../../../Headers/ShaderStructs.hlsli"

struct LightVertexInput
{
    float4 Position : POSITION;
    uint4 BoneIds : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
};


VertexOutput main(LightVertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    
    if(hasBone)
    {
        float4x4 skinMatrix = 0;
        skinMatrix += OB_BoneTransform[input.BoneIds[0]] * input.BoneWeights[0];
        skinMatrix += OB_BoneTransform[input.BoneIds[1]] * input.BoneWeights[1];
        skinMatrix += OB_BoneTransform[input.BoneIds[2]] * input.BoneWeights[2];
        skinMatrix += OB_BoneTransform[input.BoneIds[3]] * input.BoneWeights[3];
        output.Position = mul(skinMatrix, output.Position);
    }
    
    output.Position = mul(OB_Transform, input.Position);
    
    return output;
}