#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_DebugLayer.h"
#include <Modelviewer/Core/Modelviewer.h>

GfxCmd_DebugLayer::GfxCmd_DebugLayer()
{

}

void GfxCmd_DebugLayer::ExecuteAndDestroy()
{
	if(ModelViewer::GetApplicationState().filter != DebugFilter::NoFilter)
	{
		G_Buffer& gBuffer = GetGBuffer();
		gBuffer.UseDebugShader();
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
	}
}