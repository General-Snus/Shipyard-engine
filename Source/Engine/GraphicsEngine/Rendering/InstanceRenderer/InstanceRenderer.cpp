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
		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(CU::Matrix4x4<float>) * i.second.size());
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData{};
		vertexSubResourceData.pSysMem = i.second.data();

		HRESULT result;
		ComPtr<ID3D11Buffer> myInstanceBuffer;
		result = RHI::Device->CreateBuffer(&vertexBufferDesc,
			&vertexSubResourceData,
			myInstanceBuffer.GetAddressOf());

		if(FAILED(result))
		{
			AMLogger.Log("Failed to create Instance buffer");
			return;
		} 

		for(const auto& aElement : i.first.myMesh->Elements)
		{
			if(i.first.myMaterials.size() > 0)
			{
				i.first.myMaterials[0].lock()->Update();
			}

			const std::vector<ComPtr<ID3D11Buffer>> vxBuffers
			{
				aElement.VertexBuffer,
				myInstanceBuffer
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

			RHI::DrawIndexedInstanced(aElement.NumIndices,i.second.size());
		}
	} 
}

void InstanceRenderer::AddInstance(const RenderData& aRenderData)
{
	instanceRenderData.emplace(aRenderData);
}

void InstanceRenderer::UpdateInstance(const RenderData& aRenderData,const Matrix& aMatrix)
{
	instanceRenderData[aRenderData].push_back(aMatrix);
} 