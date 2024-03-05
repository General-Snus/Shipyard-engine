#include "DirectXHeader.pch.h"

#include "../CommandList.h" 
#include "../ResourceStateTracker.h"
#include "Shipyard/GPU.h"
#include "Shipyard/GpuResource.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type)
{

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandAllocator(type,IID_PPV_ARGS(&m_CommandAllocator)));

	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandList(0,type,m_CommandAllocator.Get(),
		nullptr,IID_PPV_ARGS(&m_CommandList)));

	m_ResourceStateTracker = std::make_unique<ResourceStateTracker>();
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

			TransitionResource(m_CommandList.Get(),d3d12Resource.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();

			UpdateSubresources(m_CommandList.Get(),d3d12Resource.Get(),
				uploadResource.Get(),0,0,1,&subresourceData);

			// Add references to resources so they stay in scope until the command aCommandList is reset.
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	}

	buffer.SetResource(d3d12Resource);
	buffer.CreateView(numElements,elementSize);
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
