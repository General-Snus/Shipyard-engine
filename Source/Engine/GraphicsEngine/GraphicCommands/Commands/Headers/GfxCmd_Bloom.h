#pragma once
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_Bloom : public GraphicCommandBase
{
public:
	GfxCmd_Bloom();
	void Destroy() override {};
	void ExecuteAndDestroy() override;

};

