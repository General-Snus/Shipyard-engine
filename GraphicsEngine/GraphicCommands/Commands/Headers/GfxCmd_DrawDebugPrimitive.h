#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <GraphicsEngine/DebugDrawer/DebugDrawer.h>


class GfxCmd_DrawDebugPrimitive : public GraphicCommandBase
{
private:
	Debug::DebugPrimitive myPrimitive;
	Matrix myTransform;
public:
	GfxCmd_DrawDebugPrimitive(Debug::DebugPrimitive primitive,Matrix Transform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
