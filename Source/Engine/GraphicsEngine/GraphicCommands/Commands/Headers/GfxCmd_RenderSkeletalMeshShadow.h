#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include "GfxCmd_RenderSkeletalMesh.h"

class GfxCmd_RenderSkeletalMeshShadow : public GfxCmd_RenderSkeletalMesh
{
public:
	GfxCmd_RenderSkeletalMeshShadow(const RenderData& aMesh,const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
