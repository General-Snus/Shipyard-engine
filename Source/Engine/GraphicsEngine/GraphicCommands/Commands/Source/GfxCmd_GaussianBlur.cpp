#include "GraphicsEngine.pch.h"
#include "../Headers/GfxCmd_GaussianBlur.h"



GfxCmd_GaussianBlur::GfxCmd_GaussianBlur()
{
}

void RenderTextureTo(eRenderTargets from,eRenderTargets to)
{
	const Texture* texture1 = GraphicsEngine::Get().GetTargetTextures(from).get();
	const Texture* texture2 = GraphicsEngine::Get().GetTargetTextures(to).get();

	RHI::SetRenderTarget(texture2,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,texture1);
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,nullptr);
}

void GfxCmd_GaussianBlur::ExecuteAndDestroy()
{

	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetCopyShader());
	RenderTextureTo(eRenderTargets::SceneBuffer,eRenderTargets::halfSceneBuffer);

	RenderTextureTo(eRenderTargets::halfSceneBuffer,eRenderTargets::quaterSceneBuffer1);

	//Vi har quatersize1
	//Blurr quatersize2 från quatersize1
	//quatersize blurred to half size
	//Done
	RHI::SetPixelShader(GraphicsEngine::Get().GetGaussShader());
	RenderTextureTo(eRenderTargets::quaterSceneBuffer1,eRenderTargets::quaterSceneBuffer2); 


	RHI::SetPixelShader(GraphicsEngine::Get().GetCopyShader());
	RenderTextureTo(eRenderTargets::quaterSceneBuffer2,eRenderTargets::halfSceneBuffer);
}
