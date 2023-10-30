#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_DebugLayer.h"
#include <Game/Modelviewer/Core/Modelviewer.h>

GfxCmd_DebugLayer::GfxCmd_DebugLayer()
{

}

void GfxCmd_DebugLayer::ExecuteAndDestroy()
{
	if(ModelViewer::GetApplicationState().filter != DebugFilter::NoFilter)
	{ 
		RHI::SetBlendState(nullptr);
		G_Buffer& gBuffer = GetGBuffer();
		gBuffer.UseDebugShader();
		RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::BackBuffer).get(),GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
		RHI::SetBlendState(GraphicsEngine::Get().GetAlphaBlendState());
	}
}