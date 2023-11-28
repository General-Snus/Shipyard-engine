#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_DrawDebugPrimitive.h"  

#if MOTHBALLED == 1
GfxCmd_DrawDebugPrimitive::GfxCmd_DrawDebugPrimitive(DebugDrawer::PrimitiveHandle primitive,Matrix Transform) : myPrimitive(primitive),myTransform(Transform)
{

}
void GfxCmd_DrawDebugPrimitive::ExecuteAndDestroy()
{
	LineBuffer& lineBuffer = GetLineBuffer();
	lineBuffer.Data.myTransform = myTransform;
	RHI::UpdateConstantBufferData(lineBuffer);

	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		myPrimitive.VertexBuffer,
		myPrimitive.IndexBuffer,
		sizeof(Debug::DebugVertex),
		Debug::DebugVertex::InputLayout);

	RHI::SetVertexShader(GraphicsEngine::Get().GetDebugLineVS().get());
	RHI::SetPixelShader(GraphicsEngine::Get().GetDebugLinePS().get());
	RHI::DrawIndexed(static_cast<UINT>(myPrimitive.NumIndices));
}
#endif