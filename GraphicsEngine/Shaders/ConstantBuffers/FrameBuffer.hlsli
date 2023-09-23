cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_InvView;
    float4x4 FB_Proj; 
    float FB_Time;
    float3 FB_CameraPosition;
}