#pragma once  
#include "GfxCmd_RenderMesh.h"

class GfxCmd_RenderMeshShadow : public GfxCmd_RenderMesh
{
public:
	GfxCmd_RenderMeshShadow(const std::shared_ptr<RenderData> aMesh,const Matrix& aTransform,bool instanced);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
