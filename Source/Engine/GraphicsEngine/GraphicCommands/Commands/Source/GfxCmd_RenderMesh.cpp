#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderMesh.h" 
#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>

GfxCmd_RenderMesh::GfxCmd_RenderMesh(RenderData* aData,const Matrix& aTransform) : myRenderData(aData),myTransform(aTransform)
{
	MaxExtents = aData->myMesh->MaxBox;
	MinExtents = aData->myMesh->MinBox; 
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

	GetInstanceRenderer().AddInstance(myRenderData);
	//for(const auto& aElement : myMesh->Elements)
	//{
	//	if(myMaterials.size() > 0)
	//	{
	//		myMaterials[0].lock()->Update();
	//	}
	//
	//	/*const std::vector<ComPtr<ID3D11Buffer>> vxBuffers
	//	{
	//		aElement.VertexBuffer,
	//		myMesh->
	//	}*/
	//
	//	RHI::ConfigureInputAssembler(
	//		aElement.PrimitiveTopology,
	//		aElement.VertexBuffer,
	//		aElement.IndexBuffer,
	//		aElement.Stride,
	//		Vertex::InputLayout);
	//	RHI::DrawIndexed(aElement.NumIndices);
	//}
}