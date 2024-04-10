#include "GraphicsEngine.pch.h"
#include "../Headers/GfxCmd_ToneMapPass.h"

GfxCmd_ToneMapPass::GfxCmd_ToneMapPass()
{
}

void GfxCmd_ToneMapPass::ExecuteAndDestroy()
{
	/*RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetTonemap());

	RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::BackBuffer).get(),nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target01,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateB).get());

	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);

	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target01,nullptr);*/
}
