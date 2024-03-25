#pragma once
// Reosurce wrapper for GPU resources

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0) 
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#include <memory>
#include <string>
#include <wrl/client.h> 

using namespace Microsoft::WRL;


class GpuResource
{
	friend class CommandList;
public:
	GpuResource();
	virtual ~GpuResource() { Destroy(); }

	GpuResource(const GpuResource& toCopy);
	GpuResource& operator=(const GpuResource& other);
	GpuResource& operator=(GpuResource&& other) noexcept;
	GpuResource(GpuResource& toCopy);

	inline virtual void Destroy()
	{
		m_Resource = nullptr;
		//Potential resource management here
	}


	virtual void CreateView(size_t numElements,size_t elementSize);
	void Reset();

	void SetResource(const ComPtr<ID3D12Resource>& resource);
	ComPtr<ID3D12Resource> GetResource();
	const ComPtr<ID3D12Resource>& GetResource() const;

	ID3D12Resource** GetAddressOf();

	virtual bool IsSRV() const { return false; };

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;
	void CheckFeatureSupport();

protected:
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;
	std::wstring m_ResourceName;

	ComPtr<ID3D12Resource> m_Resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
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
