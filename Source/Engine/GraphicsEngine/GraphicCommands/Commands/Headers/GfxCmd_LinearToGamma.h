#pragma once
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_LinearToGamma : public GraphicCommandBase
{
public:
	GfxCmd_LinearToGamma();
	void Destroy() override {};
	void ExecuteAndDestroy() override;

}; 