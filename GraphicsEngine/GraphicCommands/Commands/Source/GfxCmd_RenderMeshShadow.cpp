#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderMeshShadow.h"
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
#include <GraphicsEngine/Rendering/Buffers/ObjectBuffer.h> 

GfxCmd_RenderMeshShadow::GfxCmd_RenderMeshShadow(const RenderData& aMesh,const Matrix& aTransform) : GfxCmd_RenderMesh(aMesh,aTransform)
{
}

void GfxCmd_RenderMeshShadow::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);
	RHI::Context->PSSetShader(nullptr,nullptr,0);

	for(const auto& aElement : myMesh->Elements)
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

