#pragma once
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_LuminancePass : public GraphicCommandBase
{
public:
	GfxCmd_LuminancePass();
	void Destroy() override {};
	void ExecuteAndDestroy() override;

};

