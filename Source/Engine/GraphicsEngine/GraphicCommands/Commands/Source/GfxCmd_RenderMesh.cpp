#include <GraphicsEngine.pch.h>

#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>
#include <Engine\AssetManager\Objects\BaseAssets\MaterialAsset.h>
#include "../Headers/GfxCmd_RenderMesh.h"  

GfxCmd_RenderMesh::GfxCmd_RenderMesh(const std::shared_ptr<RenderData>& aData,const Matrix& aTransform,bool instanced) : myRenderData(aData),myTransform(aTransform),instanced(instanced)
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
	objectBuffer.myTransform = myTransform;
	objectBuffer.MaxExtents = MaxExtents;
	objectBuffer.MinExtents = MinExtents;
	objectBuffer.hasBone = false;
	objectBuffer.isInstanced = instanced;
	//RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer& gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();

	for (const auto& aElement : myRenderData->myMesh->Elements)
	{
		aElement;
		if (myRenderData->overrideMaterial.size())
		{
			myRenderData->overrideMaterial[0]->Update();
		}
		else
		{
			GraphicsEngine::Get().GetDefaultMaterial()->Update();
		}
		/*auto device = GPU::m_Device;
		auto& commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue->GetCommandList();


		GPU::ConfigureInputAssembler(commandList,
			aElement.PrimitiveTopology,
			aElement.VertexBuffer.GetVertexBufferView(),
			aElement.IndexResource.GetIndexBufferView()
		);
		commandList->DrawIndexedInstanced(aElement.IndexResource.GetIndexCount(),1,0,0,0);*/

		/*RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);*/
	}
}