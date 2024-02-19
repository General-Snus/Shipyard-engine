#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderSkeletalMesh.h"

#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>

GfxCmd_RenderSkeletalMesh::GfxCmd_RenderSkeletalMesh(const std::shared_ptr<RenderData>& aData,
	const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones) : GfxCmd_RenderMesh(aData,aTransform,false)
{
	aNumBones;
	assert(aNumBones < 128);
	for (size_t i = 0; i < 128; i++)
	{
		myBoneTransforms[i] = aBoneTransformList[i];
	}
}
void GfxCmd_RenderSkeletalMesh::ExecuteAndDestroy()
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

	//RHI::UpdateConstantBufferData(objectBuffer);
	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();
	//GetInstanceRenderer().AddInstance( myRenderData);
	for (const auto& aElement : myRenderData->myMesh->Elements)
	{
		if (!myRenderData->overrideMaterial.empty())
		{
			myRenderData->overrideMaterial[0]->Update();
		}aElement;
		/*RHI::ConfigureInputAssembler(aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);*/
	}
}
