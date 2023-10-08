struct DefaultVertexToPixel
{
    float4 Position : SV_POSITION;
    float4 VxColor : COLOR;
    float4 WorldPosition : WORLDPOSITION; 
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
};


struct BRDF_VS_to_PS
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};