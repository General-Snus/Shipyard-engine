#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SSAO.h" 

GfxCmd_SSAO::GfxCmd_SSAO()
{
}

void GfxCmd_SSAO::ExecuteAndDestroy()
{
	
	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetSSAOShader());
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);


	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.SetResourceTexture();

	RHI::SetRenderTarget(
		GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateA).get(),
		GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());
	  
	RHI::Draw(4);//RenderSSAO to intermediateA
	RHI::SetRenderTarget(nullptr,nullptr);




	RHI::SetPixelShader(GraphicsEngine::Get().GetEdgeBlurShader());
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateA).get());
	RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SSAO).get(), nullptr);
	RHI::Draw(4); // Render blurred SSAO to SSAO
	RHI::SetRenderTarget(nullptr,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,nullptr);
	gBuffer.UnsetResources();
}
