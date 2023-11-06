#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_SSAO : public GraphicCommandBase
{ 
public:
	GfxCmd_SSAO();
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
