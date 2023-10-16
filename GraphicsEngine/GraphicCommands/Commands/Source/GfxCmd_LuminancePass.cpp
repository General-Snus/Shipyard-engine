#include "GraphicsEngine.pch.h"
#include "../Headers/GfxCmd_LuminancePass.h"
#include <ImGui/imgui.h>

GfxCmd_LuminancePass::GfxCmd_LuminancePass()
{
}

void GfxCmd_LuminancePass::ExecuteAndDestroy()
{
	RHI::SetVertexShader(GraphicsEngine::Get().GetQuadShader());
	RHI::SetPixelShader(GraphicsEngine::Get().GetLuminanceShader()); 

	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,nullptr);
	RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateA).get(),nullptr);
	RHI::ClearRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::IntermediateA).get());

	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SceneBuffer).get());
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
