#include "DirectXHeader.pch.h"

#include "../CommandList.h" 
#include "../ResourceStateTracker.h"
#include "Shipyard/GPU.h"
#include "Shipyard/GpuResource.h"
#include "Shipyard/Texture.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type) : m_Type(type)
{

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandAllocator(type,IID_PPV_ARGS(&m_CommandAllocator)));

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandList(0,type,m_CommandAllocator.Get(),
		nullptr,IID_PPV_ARGS(&m_CommandList)));

	m_ResourceStateTracker = std::make_unique<ResourceStateTracker>();


//m_DescriptorHeap = std::make_unique<DescriptorHeap>(GPU::m_Device,
//	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
//	D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
//	Descriptors::Count);

}

void CommandList::CopyBuffer(GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,D3D12_RESOURCE_FLAGS flags)
{
	size_t bufferSize = numElements * elementSize;

	ComPtr<ID3D12Resource> d3d12Resource;
	if (bufferSize == 0)
	{
		// This will result in a NULL resource (which may be desired to define a default null resource).
	}
	else
	{
		auto var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,flags);
		Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
			&var1,
			D3D12_HEAP_FLAG_NONE,
			&var2,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&d3d12Resource)));


		// Add the resource to the global resource state tracker.
		ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(),D3D12_RESOURCE_STATE_COMMON);

		if (bufferData != nullptr)
		{
			// Create an upload resource to use as an intermediate buffer to copy the buffer resource 
			ComPtr<ID3D12Resource> uploadResource;
			auto var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
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

			// Add references to resources so they stay in scope until the command aCommandList is reset.
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
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),D3D12_RESOURCE_STATE_COMMON,stateAfter,subresource);
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

void CommandList::TransitionBarrier(const GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,UINT subresource,bool flushBarriers)
{
	TransitionBarrier(resource.GetResource(),stateAfter,subresource,flushBarriers);
}

void CommandList::SetView(eRootBindings rootParameterIndex,uint32_t descriptorOffset,const GpuResource& resource,D3D12_RESOURCE_STATES stateAfter,UINT firstSubresource,UINT numSubresources,const D3D12_SHADER_RESOURCE_VIEW_DESC* srv)
{
	if (numSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
	{
		for (uint32_t i = 0; i < numSubresources; ++i)
		{
			TransitionBarrier(resource,stateAfter,firstSubresource + i);
		}
	}
	else
	{
		TransitionBarrier(resource,stateAfter);
	} 

	//CreateBufferShaderResourceView(GPU::m_Device.Get(),resource.GetResource().Get(),resource->GetCpuHandle(Descriptors::WindowsLogo));
	 

	TrackResource(resource);
}

void CommandList::SetDescriptorTable(unsigned slot,Texture* texture)
{
	const size_t offset = texture->GetHeapOffset();

	if (offset == -1)
	{
		Logger::Warn("Texture has no heap offset");
		return;
	}

	TransitionBarrier(texture->GetResource(),D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_CommandList->SetGraphicsRootDescriptorTable(
		eRootBindings::Textures,
		GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetGpuHandle(offset));


	TrackResource(texture->GetResource());
}

void CommandList::SetRenderTargets(uint16_t numberOfTargets,Texture* renderTargets,Texture* depthBuffer)
{
	assert(numberOfTargets <= 16);
	D3D12_CPU_DESCRIPTOR_HANDLE RTVs[16] = {};
	for (size_t i = 0; i < numberOfTargets; i++)
	{ 
		TransitionBarrier(renderTargets[i].GetResource(),D3D12_RESOURCE_STATE_RENDER_TARGET);
		RTVs[i] = renderTargets[i].GetHandle();
	}
	auto copy = depthBuffer->GetHandle();
	m_CommandList->OMSetRenderTargets(numberOfTargets,RTVs,FALSE,&copy);
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
	// Flush any remaining barriers.
	FlushResourceBarriers();

	m_CommandList->Close();

	// Flush pending resource barriers.
	uint32_t numPendingBarriers = m_ResourceStateTracker->FlushPendingResourceBarriers(pendingCommandList);
	// Commit the final resource state to the global state.
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
