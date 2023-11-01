#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include "GfxCmd_RenderMesh.h"

class GfxCmd_RenderMeshShadow : public GfxCmd_RenderMesh
{
public:
	GfxCmd_RenderMeshShadow(RenderData* aMesh,const Matrix& aTransform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
