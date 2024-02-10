#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderSkybox.h"

GfxCmd_RenderSkybox::GfxCmd_RenderSkybox(std::shared_ptr<Texture> texture) : mySkyboxTexture(texture)
{
}

void GfxCmd_RenderSkybox::ExecuteAndDestroy()
{
	//RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,100,mySkyboxTexture.get());
}
