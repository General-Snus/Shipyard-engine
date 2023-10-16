
#pragma once
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_GaussianBlur : public GraphicCommandBase
{
public:
	GfxCmd_GaussianBlur();
	void Destroy() override {};
	void ExecuteAndDestroy() override;

};
