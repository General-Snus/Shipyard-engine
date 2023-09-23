cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_Transform; // 64 bytes
    float3 OB_MinExtents; // 12 bytes
    float padding1; // 4 bytes
    float3 OB_MaxExtents; // 12 bytes
    bool hasBone; // 4 bytes
    float4x4 OB_BoneTransform[128]; //64*128  
}   