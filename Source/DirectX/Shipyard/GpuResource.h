#pragma once
// Resource wrapper for GPU resources

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0) 
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#include <memory>
#include <string>
#include <unordered_map>
#include <wrl/client.h>   

#include "Gpu_fwd.h" 
using namespace Microsoft::WRL;



class GpuResource
{
	friend class CommandList;
public:
	GpuResource();
	virtual ~GpuResource() { Destroy(); }

	//GpuResource& operator=(const GpuResource& other);
	//GpuResource& operator=(GpuResource&& other) noexcept;
	//GpuResource(GpuResource& toCopy);

	virtual void Destroy()
	{
		//m_Resource->Release();
		m_Resource = nullptr;
		for (auto& [type,pair] : m_DescriptorHandles)
		{
			pair.cpuPtr.ptr = 0;
			pair.heapOffset = -1;
		}
	}

	virtual void CreateView(size_t numElements,size_t elementSize);
	virtual void SetView(ViewType view);
	virtual void SetView(ViewType view,HeapHandle handle);
	virtual void ClearView(ViewType view);

	virtual HeapHandle GetHandle(ViewType type);
	virtual HeapHandle CreateViewWithHandle(ViewType type,HeapHandle handle);
	virtual HeapHandle GetHandle() const;
	virtual int GetHeapOffset() const;

	void SetResource(const ComPtr<ID3D12Resource>& resource);
	ComPtr<ID3D12Resource> GetResource();
	const ComPtr<ID3D12Resource>& GetResource() const;
	ID3D12Resource** GetAddressOf();

	void Reset();

	bool CheckSrvSupport() const;
	bool CheckRTVSupport() const;
	bool CheckUAVSupport() const;
	bool CheckDSVSupport() const;

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;
	void CheckFeatureSupport();

protected:
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;


	DXGI_FORMAT m_Format;
	ViewType m_RecentBoundType = ViewType::SRV;
	std::unordered_map<ViewType,HeapHandle> m_DescriptorHandles;

	std::filesystem::path m_ResourceName;
	ComPtr<ID3D12Resource> m_Resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
};



class UAVResource : public GpuResource
{
public:
	UAVResource() = default;
	explicit UAVResource(std::wstring name);

	void CreateView(size_t numElements);
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandle(unsigned offset) const
	{
		return { m_Handle.ptr + (bufferSize * offset) };;
	}
	unsigned GetAllocatedElements() const
	{
		return m_AllocatedElements;
	}

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_Handle;
	unsigned m_AllocatedElements;
	const size_t bufferSize;
};

class IndexResource : public GpuResource
{
public:
	IndexResource() = default;
	explicit IndexResource(std::filesystem::path name);

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
	explicit VertexResource(std::filesystem::path name);

	uint32_t GetVertexCount() const { return m_NumVertices; }
	uint32_t GetVertexStride() const { return m_VertexStride; }
	void CreateView(size_t numElements,size_t elementSize) override;

private:
	uint32_t m_NumVertices;
	uint32_t m_VertexStride;
};
