#pragma once
#include <filesystem> 
#include <unordered_map>
#include "Enums.h" 
#include <Tools/Utilities/Ref.h>

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS) - 1)
class GpuResource
{
	friend class CommandList;

public:
	GpuResource();

	virtual ~GpuResource()
	{
		Destroy();
	}

	// GpuResource& operator=(const GpuResource& other);
	// GpuResource& operator=(GpuResource&& other) noexcept;
	// GpuResource(GpuResource& toCopy);

	virtual void Destroy();

	virtual void CreateView(size_t numElements,size_t elementSize);
	virtual void SetView(ViewType view);
	virtual void SetView(ViewType view,HeapHandle handle);
	virtual void ClearView(ViewType view);

	virtual HeapHandle GetHandle(ViewType type);
	virtual HeapHandle GetHandle(ViewType type) const;
	virtual HeapHandle CreateViewWithHandle(ViewType type,HeapHandle handle);
	virtual HeapHandle GetHandle() const;
	virtual int        GetHeapOffset() const;

	void                          SetResource(const Ref<ID3D12Resource>& resource);
	Ref<ID3D12Resource>        Resource();
	const Ref<ID3D12Resource>& Resource() const;
	ID3D12Resource**              GetAddressOf();

	void Reset();

	uint64_t GetGpuAddress() const;
	bool CheckSrvSupport() const;
	bool CheckRTVSupport() const;
	bool CheckUAVSupport() const;
	bool CheckDSVSupport() const;

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;
	void CheckFeatureSupport();
	void SetName(const wchar_t* name);

protected:
	D3D12_RESOURCE_STATES m_UsageState{};
	mutable D3D12_RESOURCE_STATES m_TransitioningState{}; // bleurgh

	DXGI_FORMAT                              m_Format;
	ViewType                                 m_RecentBoundType = ViewType::SRV;
	std::unordered_map<ViewType,HeapHandle>  m_DescriptorHandles;

	std::filesystem::path             m_ResourceName;
	Ref<ID3D12Resource>				  m_Resource;
	D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
};

