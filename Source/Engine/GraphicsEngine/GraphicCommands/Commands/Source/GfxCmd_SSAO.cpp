#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SSAO.h" 

GfxCmd_SSAO::GfxCmd_SSAO()
{
}

void GfxCmd_SSAO::ExecuteAndDestroy()
{
	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetSSAOShader());

	RHI::SetRenderTarget(
		GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SSAO).get(),
		GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());

	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.SetResourceTexture();

	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	gBuffer.UnsetResources();
	RHI::SetRenderTarget(nullptr,nullptr);
}
