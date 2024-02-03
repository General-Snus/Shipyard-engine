#include <GraphicsEngine.pch.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>
#include <Engine\AssetManager\Objects\BaseAssets\MaterialAsset.h>
#include "../Headers/GfxCmd_RenderMesh.h" 

GfxCmd_RenderMesh::GfxCmd_RenderMesh(const std::shared_ptr<RenderData> aData,const Matrix& aTransform,bool instanced) : myRenderData(aData),myTransform(aTransform),instanced(instanced)
{
	MaxExtents = aData->myMesh->MaxBox;
	MinExtents = aData->myMesh->MinBox;
}
void GfxCmd_RenderMesh::ExecuteAndDestroy()
{
	if (instanced)
	{
		GetInstanceRenderer().AddInstance(myRenderData);
		return;
	}

	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;
	objectBuffer.Data.isInstanced = instanced;
	RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();

	for (const auto& aElement : myRenderData->myMesh->Elements)
	{
		if (myRenderData->overrideMaterial.size())
		{
			myRenderData->overrideMaterial[0]->Update();
		}
		else
		{
			GraphicsEngine::Get().GetDefaultMaterial()->Update();
		}
		auto device = GPU::m_Device;
		auto commandQueue = GPU::m_CommandQueue->GetCommandQueue();
		auto commandList = GPU::m_CommandQueue->GetCommandList();


		GPU::ConfigureInputAssembler(commandList,
			aElement.PrimitiveTopology,
			aElement.m_VertexBufferView,
			aElement.m_IndexBufferView
		);
		commandList->DrawIndexedInstanced(aElement.NumIndices,1,0,0,0);

		/*RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);*/
	}
}