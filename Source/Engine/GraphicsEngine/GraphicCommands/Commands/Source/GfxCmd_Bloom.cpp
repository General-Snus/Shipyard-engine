#include "../Headers/GfxCmd_Bloom.h"
#include "GraphicsEngine.pch.h"




GfxCmd_Bloom::GfxCmd_Bloom()
{
}

void GfxCmd_Bloom::ExecuteAndDestroy()
{

	/*RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetBloomShader());

	RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateB).get(),nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SceneBuffer).get());
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target01,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::halfSceneBuffer).get());
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target01,nullptr);*/


}
