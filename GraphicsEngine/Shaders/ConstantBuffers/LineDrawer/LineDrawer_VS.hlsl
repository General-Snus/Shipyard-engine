#include "LineDrawerStructs.hlsli"
#include "../FrameBuffer.hlsli"

LineVertexToPixel main(LineVertexInput input)
{
    LineVertexToPixel result;
    
    const float4 vertexWorldSpace = mul(LB_Transform, input.Position);
    const float4 vertexViewPosition = mul(FB_InvView, vertexWorldSpace);
    const float4 vertexProjPosition = mul(FB_Proj, vertexViewPosition);
    
    result.Position = vertexProjPosition;
    result.Color = input.Color;
    
	return result;
}