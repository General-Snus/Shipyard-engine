#include "GraphicsEngine.pch.h"
#include "../Headers/GfxCmd_GaussianBlur.h"



GfxCmd_GaussianBlur::GfxCmd_GaussianBlur()
{
}



void GfxCmd_GaussianBlur::ExecuteAndDestroy()
{

	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetCopyShader());

	GraphicsEngine::Get().RenderTextureTo(eRenderTargets::IntermediateA,eRenderTargets::halfSceneBuffer);
	GraphicsEngine::Get().RenderTextureTo(eRenderTargets::halfSceneBuffer,eRenderTargets::quaterSceneBuffer1);

	RHI::SetPixelShader(GraphicsEngine::Get().GetGaussShader());
	GraphicsEngine::Get().RenderTextureTo(eRenderTargets::quaterSceneBuffer1,eRenderTargets::quaterSceneBuffer2);


	RHI::SetPixelShader(GraphicsEngine::Get().GetCopyShader());
	GraphicsEngine::Get().RenderTextureTo(eRenderTargets::quaterSceneBuffer2,eRenderTargets::halfSceneBuffer);
}
