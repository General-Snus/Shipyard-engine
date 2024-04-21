#pragma once 
#include <DirectX/Shipyard/ResourceStateTracker.h> 
#include <DirectX/Shipyard/RootSignature.h> 
#include <DirectX/XTK/DescriptorHeap.h>
#include <vector>
#include <wrl/client.h> 

enum class eHeapTypes;
class ResourceStateTracker;
class GpuResource;
class Texture;
struct ID3D12GraphicsCommandList2;
struct ID3D12Resource;

using DxCommandList = ComPtr<ID3D12GraphicsCommandList2>;
class CommandList
{
public:
	CommandList(D3D12_COMMAND_LIST_TYPE type,const std::wstring& name = L"NoName");

	void CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	void TransitionBarrier(const ComPtr<ID3D12Resource>& resource,D3D12_RESOURCE_STATES stateAfter,UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,bool flushBarriers = false);
	void TransitionBarrier(GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,bool flushBarriers = false);
	void SetDescriptorTable(unsigned slot,Texture* texture);

	void SetRenderTargets(unsigned numberOfTargets,Texture* renderTargets,Texture* depthBuffer);

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

	template<typename T>
	void SetConstantBuffer(unsigned slot,const T& constantBuffer);
	void CopyResource(const ComPtr<ID3D12Resource>& destination,const ComPtr<ID3D12Resource>& source);

	void GenerateMips(Texture& texture);
private:
	void GenerateMips_UAV(Texture& texture,DXGI_FORMAT format);

private:
	std::unique_ptr<ResourceStateTracker> m_ResourceStateTracker;
	//std::unique_ptr<DescriptorHeap> m_DescriptorHeap;
	D3D12_COMMAND_LIST_TYPE m_Type;
	DxCommandList m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	std::vector<ComPtr<ID3D12Object>> m_TrackedObjects;
};

