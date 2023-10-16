#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SetRenderTarget.h"

GfxCmd_SetRenderTarget::GfxCmd_SetRenderTarget(Texture* aRenderTarget,Texture* aDepthStencil) : renderTarget(aRenderTarget),depthStencil(aDepthStencil)
{
}

void GfxCmd_SetRenderTarget::ExecuteAndDestroy()
{
	RHI::SetRenderTarget(renderTarget,depthStencil);
}