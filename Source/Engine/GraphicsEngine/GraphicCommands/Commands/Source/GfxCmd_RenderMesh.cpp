#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_RenderMesh.h" 
#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>

GfxCmd_RenderMesh::GfxCmd_RenderMesh(RenderData* aData,const Matrix& aTransform,bool instanced) : myRenderData(aData),myTransform(aTransform),instanced(instanced)
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
	objectBuffer.Data.isInstanced = instanced;

	RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();

	if(instanced)
	{
		GetInstanceRenderer().AddInstance(myRenderData);
		return;
	} 

	for(const auto& aElement : myRenderData->myMesh->Elements)
	{
		if(myRenderData->myMaterials.size())
		{
			myRenderData->myMaterials[0].lock()->Update();
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