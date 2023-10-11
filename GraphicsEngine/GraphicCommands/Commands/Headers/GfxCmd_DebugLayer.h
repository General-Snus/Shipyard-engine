#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_DebugLayer : public GraphicCommandBase
{
public:
	GfxCmd_DebugLayer();
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
