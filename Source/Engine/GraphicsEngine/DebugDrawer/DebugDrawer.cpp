#include "GraphicsEngine.pch.h"
#include "DebugDrawer.h"

namespace Debug
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> DebugVertex::InputLayoutDescription =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ComPtr<ID3D11InputLayout> DebugVertex::InputLayout;

	DebugDrawer& DebugDrawer::Get()
	{
		static DebugDrawer singleton;
		return singleton;
	}


	DebugPrimitive::DebugPrimitive(std::vector<DebugVertex>& myVertexs,std::vector<unsigned int>& myIndices) : thickness(1.0f)
	{
		if(!RHI::CreateVertexBuffer(VertexBuffer,myVertexs))
		{
			/*myLogger.Err("Could not create vertex buffer");*/
			return;
		}
		 
		if(!RHI::CreateIndexBuffer(IndexBuffer,myIndices))
		{
			/*myLogger.Err("Could not create index buffer");*/
			return;
		}
		this->NumVertices = static_cast<unsigned>(myVertexs.size());
		this->NumIndices = static_cast<unsigned>(myIndices.size());

	}
}
