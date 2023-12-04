#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SSAO.h" 

GfxCmd_SSAO::GfxCmd_SSAO()
{
}

void GfxCmd_SSAO::ExecuteAndDestroy()
{
	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetSSAOShader());

	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.SetTexture(eGbufferTex::Position);
	gBuffer.SetTexture(eGbufferTex::Normal);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_DepthMap,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());

	RHI::SetRenderTarget(
		GraphicsEngine::Get().GetTargetTextures(eRenderTargets::halfSceneBuffer).get(),nullptr);
	RHI::ClearRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::halfSceneBuffer).get());
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);//RenderSSAO to intermediateA
	RHI::SetRenderTarget(nullptr,nullptr);

	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::halfSceneBuffer).get());
	RHI::SetPixelShader(GraphicsEngine::Get().GetEdgeBlurShader());
	RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SSAO).get(),nullptr);
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4); // Render blurred SSAO to SSAO

	RHI::SetRenderTarget(nullptr,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_DepthMap,nullptr);
	gBuffer.UnSetTexture(eGbufferTex::Position);
	gBuffer.UnSetTexture(eGbufferTex::Normal);
}
