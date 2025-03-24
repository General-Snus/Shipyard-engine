#pragma once
#include <DirectX/DX12/Graphics/GpuResource.h>

class VertexResource : public GpuResource
{
public:
	VertexResource() = default;
	explicit VertexResource(std::filesystem::path name);

	uint32_t GetVertexCount() const
	{
		return m_NumVertices;
	}

	uint32_t GetVertexStride() const
	{
		return m_VertexStride;
	}

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
	{
		return m_VertexBufferView;
	}

	void CreateView(size_t numElements, size_t elementSize) override;

private:
	uint32_t                 m_NumVertices;
	uint32_t                 m_VertexStride;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

