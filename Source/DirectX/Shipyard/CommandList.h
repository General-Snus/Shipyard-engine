#pragma once 

class ResourceStateTracker;
class GpuResource;
using DxCommandList = ComPtr<ID3D12GraphicsCommandList2>;
class CommandList
{
public:
	CommandList(D3D12_COMMAND_LIST_TYPE type);

	void CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	DxCommandList GetGraphicsCommandList() const
	{
		return m_CommandList;
	}

	void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object);

	void TrackResource(const GpuResource& res);
	void ReleaseTrackedObjects();

	void FlushResourceBarriers();

	bool Close(CommandList& pendingCommandList);

	void Close();

	void Reset();

private:

private:
	std::unique_ptr<ResourceStateTracker> m_ResourceStateTracker;
	D3D12_COMMAND_LIST_TYPE type;
	DxCommandList m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	std::vector<ComPtr<ID3D12Object>> m_TrackedObjects;
};

