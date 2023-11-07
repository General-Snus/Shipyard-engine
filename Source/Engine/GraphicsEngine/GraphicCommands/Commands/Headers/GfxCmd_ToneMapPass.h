#pragma once
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_ToneMapPass : public GraphicCommandBase
{
public:
	GfxCmd_ToneMapPass();
	void Destroy() override {};
	void ExecuteAndDestroy() override;

}; 