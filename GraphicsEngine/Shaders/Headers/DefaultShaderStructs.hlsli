struct DefaultVertexInput
{
    float4 Position : POSITION;
    float4 VxColor : COLOR;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    uint4 BoneIds : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
};
