#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_DebugLayer.h"
#include <Editor/Editor/Core/Editor.h>
//#include <Game/Modelviewer/Core/Modelviewer.h>

GfxCmd_DebugLayer::GfxCmd_DebugLayer()
{

}

void GfxCmd_DebugLayer::ExecuteAndDestroy()
{
	//TODO fix this
	//if(ModelViewer::GetApplicationState().filter != DebugFilter::NoFilter)
	if(Editor::GetApplicationState().filter != DebugFilter::NoFilter)
	{ 
		RHI::SetBlendState(nullptr);
		G_Buffer& gBuffer = GetGBuffer();
		gBuffer.UseDebugShader();
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SSAO).get());
		//gBuffer.SetTexture(eGbufferTex::Depth);
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_DepthMap,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());

		RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::BackBuffer).get(), nullptr);
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,nullptr);
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_DepthMap,nullptr);
		RHI::SetBlendState(GraphicsEngine::Get().GetAlphaBlendState());
	}
}