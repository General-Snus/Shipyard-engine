#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_SetRenderTarget : public GraphicCommandBase
{
private:
	Texture* depthStencil;
	Texture* renderTarget;
public:
	GfxCmd_SetRenderTarget(Texture* aRenderTarget,Texture* aDepthStencil);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
