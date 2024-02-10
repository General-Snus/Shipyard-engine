#include "LineDrawerStructs.hlsli"
#include "../../Headers/ShaderStructs.hlsli"

LineVertexToPixel main(LineVertexInput input)
{
    LineVertexToPixel result;
    
    //const float4 vertexWorldSpace = mul(LB_Transform, input.Position);
    const float4 vertexViewPosition = mul(g_FrameBuffer.FB_InvView, input.Position);
    const float4 vertexProjPosition = mul(g_FrameBuffer.FB_Proj, vertexViewPosition);
    
    result.Position = vertexProjPosition;
    result.Color = input.Color;
    
	return result;
}