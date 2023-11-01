#include <GraphicsEngine.pch.h>
#include "InstanceRenderer.h"
#
#include <Engine/GraphicsEngine/InterOp/RHI.h>
void InstanceRenderer::Init()
{
}

void InstanceRenderer::Execute()
{
	for(auto& i : instanceRenderData)
	{ 
		i->myMesh->UpdateInstanceBuffer();
		for(const auto& aElement : i->myMesh->Elements)
		{
			if(i->myMaterials.size())
			{
				i->myMaterials[0].lock()->Update();
			}

			const std::vector<ComPtr<ID3D11Buffer>> vxBuffers
			{
				aElement.VertexBuffer,
				i->myMesh->myInstanceBuffer
			};


			const std::vector<unsigned> vfBufferStrides
			{
				aElement.Stride,
				sizeof(Matrix)
			}; 

			RHI::ConfigureInputAssembler(
				aElement.PrimitiveTopology,
				vxBuffers,
				aElement.IndexBuffer,
				vfBufferStrides,
				Vertex::InputLayout);

			RHI::DrawIndexedInstanced(aElement.NumIndices,static_cast<unsigned>(i->myMesh->myInstances.size()));
		}
		i->myMesh->myInstances.clear();
	}
	instanceRenderData.clear();
}

void InstanceRenderer::AddInstance(RenderData* aRenderData)
{
	instanceRenderData.emplace(aRenderData);
} 