#pragma once
#include <DirectX/DX12/Graphics/GpuResource.h>

class IndexResource : public GpuResource
{
public:
	IndexResource();
	explicit IndexResource(const std::filesystem::path& name);

	uint32_t GetIndexCount() const
	{
		return m_NumIndices;
	}

	DXGI_FORMAT GetFormat() const
	{
		return m_IndexFormat;
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
	{
		return m_IndexBufferView;
	}

	void CreateView(size_t numElements, size_t elementSize) override;

private:
	uint32_t                m_NumIndices;
	DXGI_FORMAT             m_IndexFormat;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
};