#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include "GfxCmd_RenderMesh.h"
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>

class GfxCmd_RenderSkeletalMesh : public GfxCmd_RenderMesh
{
protected:
	Matrix myBoneTransforms[128];
public:
	GfxCmd_RenderSkeletalMesh(const RenderData& aMesh,const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
