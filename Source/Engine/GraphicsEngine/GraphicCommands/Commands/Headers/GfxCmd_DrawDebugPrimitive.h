#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>

#if MOTHBALLED == 1
class GfxCmd_DrawDebugPrimitive : public GraphicCommandBase
{
private:
	DebugDrawer::PrimitiveHandle myPrimitive;
	Matrix myTransform;
public:
	GfxCmd_DrawDebugPrimitive(DebugDrawer::PrimitiveHandle primitive,Matrix Transform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
#endif	