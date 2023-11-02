#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderSkeletalMeshShadow.h"

GfxCmd_RenderSkeletalMeshShadow::GfxCmd_RenderSkeletalMeshShadow(
	RenderData* aMesh,
	const Matrix& aTransform,
	const Matrix* aBoneTransformList,
	unsigned int aNumBones) :
	GfxCmd_RenderSkeletalMesh(aMesh,aTransform,aBoneTransformList,aNumBones)
{
}

void GfxCmd_RenderSkeletalMeshShadow::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = true;
	objectBuffer.Data.isInstanced = false;

	for(int i = 0; i < 128; i++)
	{
		objectBuffer.Data.myBoneTransforms[i] = myBoneTransforms[i];
	}

	RHI::UpdateConstantBufferData(objectBuffer);
	RHI::Context->PSSetShader(nullptr,nullptr,0);
	//GetInstanceRenderer().AddInstance(myRenderData);
	for(const auto& aElement :myRenderData->myMesh->Elements)
	{
		RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}
