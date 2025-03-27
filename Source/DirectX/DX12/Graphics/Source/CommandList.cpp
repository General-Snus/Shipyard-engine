#include "DirectXHeader.pch.h"

#include "Graphics/CommandList.h"
#include <ResourceUploadBatch.h>
#include <Graphics/RootSignature.h>
#include "Graphics/GPU.h"
#include "Graphics/GpuResource.h"
#include "Graphics/ResourceStateTracker.h"
#include "Graphics/Resources/Texture.h"
#include "Graphics/Resources/IndexBuffer.h"
#include "Graphics/Resources/VertexBuffer.h"
#include "Graphics/MIPS/GenerateMipsPSO.h"

CommandList::CommandList(const DeviceType& device,D3D12_COMMAND_LIST_TYPE type,const std::wstring& name)
	: m_Type(type),m_Device(device) {
	Helpers::ThrowIfFailed(m_Device->CreateCommandAllocator(type,IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	Helpers::ThrowIfFailed(m_Device->CreateCommandList(0,type,m_CommandAllocator.Get(),nullptr,IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	m_CommandAllocator->SetName((name + L"Allocator").c_str());
	m_CommandList->SetName((name + L"List").c_str());
	m_ResourceStateTracker = std::make_unique<ResourceStateTracker>();
}

void CommandList::CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,
							 D3D12_RESOURCE_FLAGS flags,CD3DX12_HEAP_PROPERTIES HeapProperties)
{
	OPTICK_GPU_EVENT("CopyBuffer");
	const size_t bufferSize = numElements * elementSize;

	Ref<ID3D12Resource> d3d12Resource;
	if(bufferSize != 0) {
		{
			const auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,flags);
			Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(&HeapProperties,D3D12_HEAP_FLAG_NONE,&var2,
				D3D12_RESOURCE_STATE_COMMON,nullptr,
				IID_PPV_ARGS(d3d12Resource.GetAddressOf())));
		}

		ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(),D3D12_RESOURCE_STATE_COMMON);

		if(bufferData != nullptr) {
			Ref<ID3D12Resource> uploadResource;
			const auto             var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			const auto             var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
			Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(&var1,D3D12_HEAP_FLAG_NONE,&var2,
				D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,
				IID_PPV_ARGS(uploadResource.GetAddressOf())));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			m_ResourceStateTracker->TransitionResource(d3d12Resource.Get(),D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();

			if(UpdateSubresources(m_CommandList.Get(),d3d12Resource.Get(),uploadResource.Get(),0,0,1,
				&subresourceData) == 0) {
				throw std::exception("UpdateSubresources failed");
			}
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	}

	buffer.SetResource(d3d12Resource);
	buffer.CreateView(numElements,elementSize);
}

template <typename T>
void CommandList::SetConstantBuffer(unsigned slot,const T& constantBuffer) {
	OPTICK_GPU_EVENT("SetConstantBuffer");
	const auto& alloc = GPUInstance.m_GraphicsMemory->AllocateConstant<T>(constantBuffer);
	m_CommandList->SetGraphicsRootConstantBufferView(slot,alloc.GpuAddress());
}

void CommandList::CopyResource(const Ref<ID3D12Resource>& destination,const Ref<ID3D12Resource>& source) {
	OPTICK_GPU_EVENT("CopyResource");
	TransitionBarrier(destination,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(source,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_SOURCE);

	FlushResourceBarriers();
	m_CommandList->CopyResource(destination.Get(),source.Get());

	TrackResource(destination);
	TrackResource(source);
}

void CommandList::TransitionBarrier(const Ref<ID3D12Resource>& resource,D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter,
	unsigned                      subresource,bool               flushBarriers) {
	TransitionBarrier(resource.Get(),stateBefore,stateAfter,subresource,flushBarriers);
}

void CommandList::TransitionBarrier(ID3D12Resource* resource,D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter,
	unsigned                      subresource,bool               flushBarriers) {
	OPTICK_GPU_EVENT("TransitionBarrier");
	if(resource) {
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource,stateBefore,stateAfter,subresource);
		m_ResourceStateTracker->ResourceBarrier(barrier);
	} else {
		LOGGER.Err("Could not transition resource");
	}

	if(flushBarriers) {
		FlushResourceBarriers();
	}
}

void CommandList::TransitionBarrier(GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,unsigned subresource,bool flushBarriers) {
	OPTICK_EVENT();
	TransitionBarrier(resource.GetResource(),resource.m_TransitioningState != -1 ? resource.m_TransitioningState : D3D12_RESOURCE_STATE_COMMON,stateAfter,subresource,flushBarriers);
	resource.m_TransitioningState = stateAfter;
}

void CommandList::SetDescriptorTable(unsigned slot,Texture* texture) {
	OPTICK_EVENT();
	const size_t offset = texture->GetHandle(ViewType::SRV).heapOffset;

	if(offset == -1) {
		LOGGER.Warn("Texture has no heap offset: " + texture->GetName());
		return;
	}

	TransitionBarrier(*texture,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_CommandList->SetGraphicsRootDescriptorTable(
		slot,
		GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetGpuHandle(offset));

	TrackResource(texture->GetResource());
}

void CommandList::SetRenderTargets(unsigned numberOfTargets,Texture* renderTargets,Texture* depthBuffer) {
	OPTICK_GPU_EVENT("SetRenderTargets");
	assert(numberOfTargets <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	D3D12_CPU_DESCRIPTOR_HANDLE RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	if(renderTargets) {
		for(size_t i = 0; i < numberOfTargets; i++) {
			TransitionBarrier(renderTargets[i],D3D12_RESOURCE_STATE_RENDER_TARGET);
			RTVs[i] = renderTargets[i].GetHandle(ViewType::RTV).cpuPtr;
		}
	}
	if(depthBuffer) {
		const auto dsv = depthBuffer->GetHandle(ViewType::DSV).cpuPtr;
		TransitionBarrier(*depthBuffer,D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_CommandList->OMSetRenderTargets(numberOfTargets,RTVs,FALSE,&dsv);
	} else {
		m_CommandList->OMSetRenderTargets(numberOfTargets,RTVs,FALSE,nullptr);
	}
}

void CommandList::ClearRenderTargets(unsigned numberOfTargets,Texture* rtv) {
	OPTICK_EVENT();
	for(unsigned i = 0; i < numberOfTargets; ++i) {
		m_CommandList->ClearRenderTargetView(rtv[i].GetHandle(ViewType::RTV).cpuPtr,&rtv->m_ClearColor.x,0,
			nullptr);
	}
}

void CommandList::ClearRenderTarget(Texture rtv) {
	OPTICK_EVENT();
	m_CommandList->ClearRenderTargetView(rtv.GetHandle(ViewType::RTV).cpuPtr,&rtv.m_ClearColor.x,0,nullptr);
}

void CommandList::SetPipelineState(const PSO& pso) {
	const auto& pipelineState = pso.GetPipelineState(); 
	m_CommandList->SetPipelineState(pipelineState.Get());
	TrackResource(pipelineState);
}

void CommandList::SetViewports(const D3D12_VIEWPORT& viewPort,unsigned num) {
	m_CommandList->RSSetViewports(num,&viewPort);
}

void CommandList::SetSissorRect(const D3D12_RECT& sissorRect,unsigned num) {
	m_CommandList->RSSetScissorRects(num,&sissorRect);
}

void CommandList::ConfigureInputAssembler(D3D_PRIMITIVE_TOPOLOGY topology,IndexResource& indexResource) {
	OPTICK_GPU_EVENT("ConfigureInputAssembler");
	m_CommandList->IASetPrimitiveTopology(topology);
	TransitionBarrier(indexResource,D3D12_RESOURCE_STATE_INDEX_BUFFER);
	const auto& indexView = indexResource.GetIndexBufferView();
	m_CommandList->IASetIndexBuffer(&indexView);
	TrackResource(indexResource.GetResource());
}

void CommandList::ConfigureInputAssembler(D3D_PRIMITIVE_TOPOLOGY topology,VertexResource& VertexResource,
	IndexResource& indexResource) {
	OPTICK_GPU_EVENT("ConfigureInputAssembler");
	m_CommandList->IASetPrimitiveTopology(topology);
	TransitionBarrier(indexResource,D3D12_RESOURCE_STATE_INDEX_BUFFER);
	const auto& indexView = indexResource.GetIndexBufferView();
	const auto& vertexView = VertexResource.GetVertexBufferView();
	m_CommandList->IASetIndexBuffer(&indexView);
	m_CommandList->IASetVertexBuffers(0,1,&vertexView);
	TrackResource(indexResource.GetResource());
}

void CommandList::TrackResource(const Ref<ID3D12Object> object) {
	OPTICK_EVENT();
	m_TrackedObjects.push_back(object);
}
void CommandList::TrackResource(const GpuResource& object) {
	OPTICK_EVENT();
	m_TrackedObjects.push_back(object.GetResource());
}
void CommandList::ReleaseTrackedObjects() {
	OPTICK_EVENT();
	m_TrackedObjects.clear();
}

void CommandList::FlushResourceBarriers() {
	m_ResourceStateTracker->FlushResourceBarriers(*this);
}

bool CommandList::Close(CommandList& pendingCommandList) {
	OPTICK_EVENT();
	FlushResourceBarriers();

	m_CommandList->Close();

	const uint32_t numPendingBarriers = m_ResourceStateTracker->FlushPendingResourceBarriers(pendingCommandList);
	m_ResourceStateTracker->CommitFinalResourceStates();

	return numPendingBarriers > 0;
}

void CommandList::Close() {
	FlushResourceBarriers();
	m_CommandList->Close();
}

void CommandList::Reset() {
	Helpers::ThrowIfFailed(m_CommandAllocator->Reset());
	Helpers::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(),nullptr));

	m_ResourceStateTracker->Reset();

	ReleaseTrackedObjects();
}
