#include "DirectXHeader.pch.h"

#include "../CommandList.h" 
#include "../ResourceStateTracker.h"
#include "Shipyard/GPU.h"
#include "Shipyard/GpuResource.h"
#include "Shipyard/Texture.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type,const std::wstring& name) : m_Type(type)
{

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandAllocator(type,IID_PPV_ARGS(&m_CommandAllocator)));

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandList(0,type,m_CommandAllocator.Get(),
		nullptr,IID_PPV_ARGS(&m_CommandList)));

	m_CommandAllocator->SetName((name + L"Allocator").c_str());
	m_CommandList->SetName((name + L"List").c_str());
	m_ResourceStateTracker = std::make_unique<ResourceStateTracker>();
}

void CommandList::CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,D3D12_RESOURCE_FLAGS flags)
{
	const size_t bufferSize = numElements * elementSize;

	ComPtr<ID3D12Resource> d3d12Resource;
	if (bufferSize == 0)
	{
		// This will result in a NULL resource (which may be desired to define a default null resource).
	}
	else
	{
		{
			const auto var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			const auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,flags);
			Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
				&var1,
				D3D12_HEAP_FLAG_NONE,
				&var2,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&d3d12Resource)));
		}

		ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(),D3D12_RESOURCE_STATE_COMMON);

		if (bufferData != nullptr)
		{
			ComPtr<ID3D12Resource> uploadResource;
			const auto var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			const auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
			Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
				&var1,
				D3D12_HEAP_FLAG_NONE,
				&var2,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&uploadResource)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			m_ResourceStateTracker->TransitionResource(d3d12Resource.Get(),D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();

			if (UpdateSubresources(m_CommandList.Get(),d3d12Resource.Get(),
				uploadResource.Get(),0,0,1,&subresourceData) == 0)
			{
				throw std::exception("UpdateSubresources failed");
			}
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	}

	buffer.SetResource(d3d12Resource);
	buffer.CreateView(numElements,elementSize);
}

void CommandList::TransitionBarrier(const ComPtr<ID3D12Resource>& resource,D3D12_RESOURCE_STATES stateAfter,UINT subresource,bool flushBarriers)
{
	if (resource)
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),D3D12_RESOURCE_STATE_COMMON,stateAfter,subresource);
		m_ResourceStateTracker->ResourceBarrier(barrier);
	}
	else
	{
		Logger::Err("Could not transition resource");
	}

	if (flushBarriers)
	{
		FlushResourceBarriers();
	}
}

void CommandList::TransitionBarrier(GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,UINT subresource,bool flushBarriers)
{
	resource.m_TransitioningState = stateAfter;
	TransitionBarrier(resource.GetResource(),stateAfter,subresource,flushBarriers);
	resource.m_TransitioningState = stateAfter;
}

void CommandList::SetDescriptorTable(unsigned slot,Texture* texture)
{
	const size_t offset = texture->GetHandle(ViewType::SRV).heapOffset;

	if (offset == -1)
	{
		Logger::Warn("Texture has no heap offset: " + texture->GetName());
		return;
	}

	TransitionBarrier(texture->GetResource(),D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_CommandList->SetGraphicsRootDescriptorTable(
		slot,
		GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetGpuHandle(offset));


	TrackResource(texture->GetResource());
}

void CommandList::SetRenderTargets(unsigned numberOfTargets,Texture* renderTargets,Texture* depthBuffer)
{
	assert(numberOfTargets <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	D3D12_CPU_DESCRIPTOR_HANDLE RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	if (renderTargets)
	{
		for (size_t i = 0; i < numberOfTargets; i++)
		{
			TransitionBarrier(renderTargets[i],D3D12_RESOURCE_STATE_RENDER_TARGET);
			RTVs[i] = renderTargets[i].GetHandle(ViewType::RTV).cpuPtr;
		}
	}
	if (depthBuffer)
	{

		const auto dsv = depthBuffer->GetHandle(ViewType::DSV).cpuPtr;
		TransitionBarrier(*depthBuffer,D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_CommandList->OMSetRenderTargets(numberOfTargets,RTVs,FALSE,&dsv);
	}
	else
	{
		m_CommandList->OMSetRenderTargets(numberOfTargets,RTVs,FALSE,nullptr);
	}
}

void CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object)
{
	m_TrackedObjects.push_back(object);
}

void CommandList::TrackResource(const GpuResource& res)
{
	TrackResource(res.GetResource());
}

void CommandList::ReleaseTrackedObjects()
{
	m_TrackedObjects.clear();
}

void CommandList::FlushResourceBarriers()
{
	m_ResourceStateTracker->FlushResourceBarriers(*this);
}

bool CommandList::Close(CommandList& pendingCommandList)
{
	FlushResourceBarriers();

	m_CommandList->Close();

	const uint32_t numPendingBarriers = m_ResourceStateTracker->FlushPendingResourceBarriers(pendingCommandList);
	m_ResourceStateTracker->CommitFinalResourceStates();

	return numPendingBarriers > 0;
}

void CommandList::Close()
{
	FlushResourceBarriers();
	m_CommandList->Close();
}

void CommandList::Reset()
{
	Helpers::ThrowIfFailed(m_CommandAllocator->Reset());
	Helpers::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(),nullptr));

	m_ResourceStateTracker->Reset();

	ReleaseTrackedObjects();
}
