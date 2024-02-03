#include <GraphicsEngine.pch.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h> 
#include "../Headers/GfxCmd_RenderMeshShadow.h"

GfxCmd_RenderMeshShadow::GfxCmd_RenderMeshShadow(const std::shared_ptr<RenderData> aMesh,const Matrix& aTransform,bool instanced) : GfxCmd_RenderMesh(aMesh,aTransform,instanced)
{
}

void GfxCmd_RenderMeshShadow::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;
	objectBuffer.Data.isInstanced = instanced;

	RHI::UpdateConstantBufferData(objectBuffer);
	RHI::Context->PSSetShader(nullptr,nullptr,0);

	if (instanced)
	{
		GetInstanceRenderer().AddInstance(myRenderData);
		return;
	}
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

