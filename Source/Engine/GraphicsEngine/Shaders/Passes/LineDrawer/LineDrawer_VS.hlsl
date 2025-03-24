#include "LineDrawerStructs.hlsli"
#include "../../Headers/ShaderStructs.hlsli"

LineVertexToPixel main(uint vertexId : SV_VertexID)
{ 
	LineVertexInput input = meshHeap[g_defaultMaterial.vertexBufferIndex].Load<LineVertexInput>((g_defaultMaterial.vertexOffset + vertexId) * sizeof(LineVertexInput));
	LineVertexToPixel result; 
    const float4 vertexViewPosition = mul(g_FrameBuffer.FB_InvView, input.Position);
    const float4 vertexProjPosition = mul(g_FrameBuffer.FB_Proj, vertexViewPosition);
    
    result.Position = vertexProjPosition;
    result.Color = input.Color;
    
	return result;
}