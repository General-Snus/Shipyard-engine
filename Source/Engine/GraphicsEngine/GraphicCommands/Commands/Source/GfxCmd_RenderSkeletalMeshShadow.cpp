#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderSkeletalMeshShadow.h"

GfxCmd_RenderSkeletalMeshShadow::GfxCmd_RenderSkeletalMeshShadow(
	const std::shared_ptr<RenderData> aMesh,
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

	for (int i = 0; i < 128; i++)
	{
		objectBuffer.Data.myBoneTransforms[i] = myBoneTransforms[i];
	}
	/*RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER,REG_ObjectBuffer,objectBuffer);
	RHI::UpdateConstantBufferData(objectBuffer);*/
	//GetInstanceRenderer().AddInstance(myRenderData);
	for (const auto& aElement : myRenderData->myMesh->Elements)
	{
		aElement;
		/*RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);*/
	}
}
