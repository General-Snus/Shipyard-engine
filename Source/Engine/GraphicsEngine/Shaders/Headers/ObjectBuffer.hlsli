#include "../Registers.h" 
#ifndef ObjectSTRUCTS
#define ObjectSTRUCTS

struct ObjectBuffer
{
    float4x4 OB_Transform; // 64 bytes
    float3 OB_MinExtents; // 12 bytes
    bool hasBone; // 4 bytes
    float3 OB_MaxExtents; // 12 bytes
    bool OB_Instanced;
    //float4x4 OB_BoneTransform[128]; //64*128  
};
ConstantBuffer<ObjectBuffer> g_ObjectBuffer : register(HLSL_REG_ObjectBuffer);
#endif
