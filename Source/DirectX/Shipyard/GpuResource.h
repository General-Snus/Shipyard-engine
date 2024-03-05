#pragma once
// Reosurce wrapper for GPU resources
#include <DirectX\directx\d3d12.h>

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0) 
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#include <memory>
#include <string>
#include <wrl/client.h> 

using namespace Microsoft::WRL;


class GpuResource
{

public:
	GpuResource();
	GpuResource(const GpuResource& copy);
	virtual ~GpuResource() { Destroy(); }



	inline virtual void Destroy()
	{
		m_pResource = nullptr;
		//Potential resource management here
	}


	virtual void CreateView(size_t numElements,size_t elementSize);


	ID3D12Resource* operator->();
	const ID3D12Resource* operator->() const;

	void SetResource(ComPtr<ID3D12Resource> resource);
	ComPtr<ID3D12Resource> GetResource();
	const ComPtr<ID3D12Resource>& GetResource() const;

	ID3D12Resource** GetAddressOf();

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

	void CheckFeatureSupport();

protected:
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;
	std::wstring m_ResourceName;

	ComPtr<ID3D12Resource> m_pResource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
	std::unique_ptr<D3D12_CLEAR_VALUE> m_d3d12ClearValue;
};

class IndexResource : public GpuResource
{
public:
	IndexResource() = default;
	explicit IndexResource(std::wstring name);

	uint32_t GetIndexCount() const { return m_NumIndices; }
	DXGI_FORMAT GetFormat() const { return m_IndexFormat; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
	{
		return m_IndexBufferView;
	}
	void CreateView(size_t numElements,size_t elementSize) override;

private:
	uint32_t m_NumIndices;
	DXGI_FORMAT m_IndexFormat;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
};

class VertexResource : public GpuResource
{
public:
	VertexResource() = default;
	explicit VertexResource(std::wstring name);

	uint32_t GetVertexCount() const { return m_NumVertices; }
	uint32_t GetVertexStride() const { return m_VertexStride; }
	D3D12_VERTEX_BUFFER_VIEW  GetVertexBufferView() const
	{
		return m_VertexBufferView;
	}
	void CreateView(size_t numElements,size_t elementSize) override;

private:
	uint32_t m_NumVertices;
	uint32_t m_VertexStride;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};
