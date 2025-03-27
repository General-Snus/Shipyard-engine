#pragma once
#include <string>
#include <vector>
#include <DirectX/DX12/Graphics/Gpu_fwd.h>
#include <Tools/Utilities/Ref.h>

#ifndef incCommandList
#define incCommandList

class PSO;

class CommandList {
public:
	CommandList(const DeviceType& device,D3D12_COMMAND_LIST_TYPE type,const std::wstring& name = L"NoName");

	void CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,
		D3D12_RESOURCE_FLAGS    flags = D3D12_RESOURCE_FLAG_NONE,
		CD3DX12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT));

	void TransitionBarrier(const Ref<ID3D12Resource>& resource,D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter,
		unsigned subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,bool flushBarriers = false);

	void TransitionBarrier(ID3D12Resource* resource,D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter,
		unsigned                      subresource,bool               flushBarriers);

	void TransitionBarrier(GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,
		unsigned subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,bool flushBarriers = false);

	void SetDescriptorTable(unsigned slot,Texture* texture);

	template <typename T>
	void AllocateBuffer(eRootBindings binding,const T& var) {
		const auto& alloc = GPUInstance.m_GraphicsMemory->AllocateConstant(var);
		m_CommandList->SetGraphicsRootConstantBufferView(static_cast<int>(binding),alloc.GpuAddress());
	}

	void SetRenderTargets(unsigned numberOfTargets,Texture* renderTargets,Texture* depthBuffer);
	void ClearRenderTargets(unsigned numberOfTargets,Texture* rtv);

	void ClearRenderTarget(Texture rtv);
	void SetPipelineState(const PSO& pso);

	void SetViewports(const D3D12_VIEWPORT& viewPort,unsigned num = 1);
	void SetSissorRect(const D3D12_RECT& sissorRect,unsigned num = 1);
	void ConfigureInputAssembler(D3D_PRIMITIVE_TOPOLOGY topology,
		IndexResource& indexResource);

	void ConfigureInputAssembler(D3D_PRIMITIVE_TOPOLOGY topology,VertexResource& VertexResource,
		IndexResource& indexResource);

	DxCommandList GetGraphicsCommandList() const {
		return m_CommandList;
	}

	void TrackResource(const Ref<ID3D12Object> object);
	void TrackResource(const GpuResource & object);


	void ReleaseTrackedObjects();
	void FlushResourceBarriers();

	bool Close(CommandList& pendingCommandList);

	void Close();

	void Reset();

	template <typename T>
	void SetConstantBuffer(unsigned slot,const T& constantBuffer);
	void CopyResource(const Ref<ID3D12Resource>& destination,const Ref<ID3D12Resource>& source);

	void GenerateMips(Texture& texture);

private:
	void GenerateMips_UAV(Texture& texture,DXGI_FORMAT format);

	std::unique_ptr<ResourceStateTracker> m_ResourceStateTracker;
	// std::unique_ptr<DescriptorHeap> m_DescriptorHeap;
	D3D12_COMMAND_LIST_TYPE           m_Type;
	DxCommandList                     m_CommandList;
	DeviceType                        m_Device;
	Ref<ID3D12CommandAllocator>    m_CommandAllocator;
	std::vector<Ref<ID3D12Object>> m_TrackedObjects;
};

#endif // incCommandList
