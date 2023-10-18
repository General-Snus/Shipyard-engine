#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_RenderSkybox : public GraphicCommandBase
{
private:
	std::shared_ptr<Texture> mySkyboxTexture;
public:
	GfxCmd_RenderSkybox(std::shared_ptr<Texture> texture);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};