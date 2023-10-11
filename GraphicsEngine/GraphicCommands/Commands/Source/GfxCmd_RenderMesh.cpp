#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderMesh.h" 
#include <GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>

GfxCmd_RenderMesh::GfxCmd_RenderMesh(const RenderData& aData,const Matrix& aTransform) : myMesh(aData.myMesh)
{ 
	myTransform = aTransform;
	MaxExtents = aData.myMesh->MaxBox;
	MinExtents = aData.myMesh->MinBox;

	for(auto& aMaterial : aData.myMaterials)
	{
		myMaterials.push_back(aMaterial);
	}
}
void GfxCmd_RenderMesh::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();

	for(const auto& aElement : myMesh->Elements)
	{
		if(myMaterials.size() > 0)
		{
			myMaterials[0].lock()->Update();
		}
		RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}