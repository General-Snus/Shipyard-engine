#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include "GfxCmd_RenderMesh.h"
struct RenderData;
class GfxCmd_RenderSkeletalMesh : public GfxCmd_RenderMesh
{
protected:
	Matrix myBoneTransforms[128];
public:
	void Destroy() override {};
	GfxCmd_RenderSkeletalMesh(const std::shared_ptr<RenderData>& aData,const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones);
	void ExecuteAndDestroy() override;
};
