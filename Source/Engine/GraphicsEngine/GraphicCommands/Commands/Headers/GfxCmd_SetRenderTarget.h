#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_SetRenderTarget : public GraphicCommandBase
{
private:
	const Texture* depthStencil;
	const Texture* renderTarget;
public:
	GfxCmd_SetRenderTarget(const Texture* aRenderTarget,const Texture* aDepthStencil);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
