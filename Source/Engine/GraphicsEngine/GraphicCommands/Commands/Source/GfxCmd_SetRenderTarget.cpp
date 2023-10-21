#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SetRenderTarget.h"

GfxCmd_SetRenderTarget::GfxCmd_SetRenderTarget(const Texture* aRenderTarget,const Texture* aDepthStencil)
	: depthStencil(aDepthStencil),renderTarget(aRenderTarget)
{
}

void GfxCmd_SetRenderTarget::ExecuteAndDestroy()
{
	RHI::SetRenderTarget(renderTarget,depthStencil);
}