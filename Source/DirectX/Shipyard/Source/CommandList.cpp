#include "DirectX/DirectXHeader.pch.h"

#include <DirectX/XTK/ResourceUploadBatch.h> 
#include "DirectX/Shipyard/CommandList.h"   
#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/GpuResource.h"
#include "DirectX/Shipyard/MIPS/GenerateMipsPSO.h"
#include "DirectX/Shipyard/ResourceStateTracker.h"
#include "DirectX/Shipyard/Texture.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type, const std::wstring& name) : m_Type(type)
{
	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator))); 
	Helpers::ThrowIfFailed(GPU::m_Device->CreateCommandList(0, type, m_CommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&m_CommandList)));

	m_CommandAllocator->SetName((name + L"Allocator").c_str());
	m_CommandList->SetName((name + L"List").c_str());
	m_ResourceStateTracker = std::make_unique<ResourceStateTracker>();
}

void CommandList::CopyBuffer(GpuResource& buffer, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
	OPTICK_GPU_EVENT("CopyBuffer");
	const size_t bufferSize = numElements * elementSize;

	ComPtr<ID3D12Resource> d3d12Resource;
	if (bufferSize != 0)
	{
		{
			const auto var1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			const auto var2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
			Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
				&var1,
				D3D12_HEAP_FLAG_NONE,
				&var2,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&d3d12Resource)));
		}

		ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

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

			m_ResourceStateTracker->TransitionResource(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarriers();

			if (UpdateSubresources(m_CommandList.Get(), d3d12Resource.Get(),
				uploadResource.Get(), 0, 0, 1, &subresourceData) == 0)
			{
				throw std::exception("UpdateSubresources failed");
			}
			TrackResource(uploadResource);
		}
		TrackResource(d3d12Resource);
	}

	buffer.SetResource(d3d12Resource);
	buffer.CreateView(numElements, elementSize);
}



template<typename T>
void CommandList::SetConstantBuffer(unsigned slot, const T& constantBuffer)
{
	OPTICK_GPU_EVENT("SetConstantBuffer");
	const auto& alloc = GPU::m_GraphicsMemory->AllocateConstant<T>(constantBuffer);
	m_CommandList->SetGraphicsRootConstantBufferView(slot, alloc.GpuAddress());
}

void CommandList::CopyResource(const ComPtr<ID3D12Resource>& destination, const ComPtr<ID3D12Resource>& source)
{
	OPTICK_GPU_EVENT("CopyResource");
	TransitionBarrier(destination, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(source, D3D12_RESOURCE_STATE_COPY_SOURCE);


	FlushResourceBarriers();
	m_CommandList->CopyResource(destination.Get(), source.Get());

	TrackResource(destination);
	TrackResource(source);
}

//void CommandList::GenerateMips(Texture& texture)
//{
//	if (m_Type == D3D12_COMMAND_LIST_TYPE_COPY)
//	{
//		const auto computeList = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->GetCommandList();
//		computeList->GenerateMips(texture);
//		return;
//	}
//
//	auto resource = texture.GetResource();
//
//	// If the texture doesn't have a valid resource, do nothing.
//	if (!resource) { return; }
//	auto resourceDesc = resource->GetDesc();
//
//	// If the texture only has a single mip level (level 0)
//	// do nothing.
//	if (resourceDesc.MipLevels == 1) return;
//	// Currently, only non-multi-sampled 2D textures are supported.
//	if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
//		resourceDesc.DepthOrArraySize != 1 ||
//		resourceDesc.SampleDesc.Count > 1)
//	{
//		return;
//	}
//
//	ComPtr<ID3D12Resource> uavResource = resource;
//	// Create an alias of the original resource.
//	// This is done to perform a GPU copy of resources with different formats.
//	// BGR -> RGB texture copies will fail GPU validation unless performed 
//	// through an alias of the BRG resource in a placed heap.
//	ComPtr<ID3D12Resource> aliasResource;
//
//	// If the passed-in resource does not allow for UAV access
//	// then create a staging resource that is used to generate
//	// the mipmap chain.
//	if (!texture.CheckUAVSupport() ||
//		(resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0)
//	{
//		auto device = GPU::m_Device;
//
//		// Describe an alias resource that is used to copy the original texture.
//		auto aliasDesc = resourceDesc;
//		// Placed resources can't be render targets or depth-stencil views.
//		aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
//		aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
//
//		// Describe a UAV compatible resource that is used to perform
//		// mipmapping of the original texture.
//		auto uavDesc = aliasDesc;   // The flags for the UAV description must match that of the alias description.
//		uavDesc.Format = Helpers::GetUAVCompatableFormat(resourceDesc.Format);
//
//		D3D12_RESOURCE_DESC resourceDescs[] = {
//			aliasDesc,
//			uavDesc
//		};
//
//		// Create a heap that is large enough to store a copy of the original resource.
//		auto allocationInfo = device->GetResourceAllocationInfo(0,_countof(resourceDescs),resourceDescs);
//
//		D3D12_HEAP_DESC heapDesc = {};
//		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
//		heapDesc.Alignment = allocationInfo.Alignment;
//		heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
//		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//		heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
//
//		ComPtr<ID3D12Heap> heap;
//		Helpers::ThrowIfFailed(device->CreateHeap(&heapDesc,IID_PPV_ARGS(&heap)));
//		TrackResource(heap);
//
//		// Create a placed resource that matches the description of the 
//		// original resource. This resource is used to copy the original 
//		// texture to the UAV compatible resource.
//		Helpers::ThrowIfFailed(device->CreatePlacedResource(
//			heap.Get(),
//			0,
//			&aliasDesc,
//			D3D12_RESOURCE_STATE_COMMON,
//			nullptr,
//			IID_PPV_ARGS(&aliasResource)
//		));
//
//		ResourceStateTracker::AddGlobalResourceState(aliasResource.Get(),D3D12_RESOURCE_STATE_COMMON);
//		TrackResource(aliasResource);
//
//		// Create a UAV compatible resource in the same heap as the alias
//		// resource.
//		Helpers::ThrowIfFailed(device->CreatePlacedResource(
//			heap.Get(),
//			0,
//			&uavDesc,
//			D3D12_RESOURCE_STATE_COMMON,
//			nullptr,
//			IID_PPV_ARGS(&uavResource)
//		));
//
//		ResourceStateTracker::AddGlobalResourceState(uavResource.Get(),D3D12_RESOURCE_STATE_COMMON);
//		TrackResource(uavResource);
//
//		{
//			auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr,aliasResource.Get());
//			m_ResourceStateTracker->ResourceBarrier(barrier);
//		}
//
//		CopyResource(aliasResource,resource);
//
//		{
//			auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(aliasResource.Get(),uavResource.Get());
//			m_ResourceStateTracker->ResourceBarrier(barrier);
//		}
//	}
//
//	Texture texture1;
//	texture1.SetResource(uavResource);
//	texture1.SetView(ViewType::UAV);
//
//	// Generate mips with the UAV compatible resource.
//	GenerateMips_UAV(texture1,resourceDesc.Format);
//
//	if (aliasResource)
//	{
//		auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(uavResource.Get(),aliasResource.Get());
//		m_ResourceStateTracker->ResourceBarrier(barrier);
//		CopyResource(resource,aliasResource);
//	}
//}
//
//void CommandList::GenerateMips_UAV(Texture& texture,DXGI_FORMAT format)
//{
//	auto& m_GenerateMipsPSO = PSOCache::GetState(PSOCache::ePipelineStateID::GenerateMips);
//
//
//	m_CommandList->SetPipelineState(m_GenerateMipsPSO->GetPipelineState().Get());
//	auto root = PSOCache::m_MipRootSignature->GetRootSignature().Get();
//	m_CommandList->SetComputeRootSignature(root);
//
//	TrackResource(root);
//
//	GenerateMipsCB generateMipsCB;
//	generateMipsCB.IsSRGB = true;// Texture::IsSRGBFormat(format);
//
//	auto resource = texture.GetResource();
//	auto resourceDesc = resource->GetDesc();
//
//	// Create an SRV that uses the format of the original texture.
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Format = format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  // Only 2D textures are supported (this was checked in the calling function).
//	srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
//
//	for (uint32_t srcMip = 0; srcMip < resourceDesc.MipLevels - 1u; )
//	{
//		uint64_t srcWidth = resourceDesc.Width >> srcMip;
//		uint32_t srcHeight = resourceDesc.Height >> srcMip;
//		uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
//		uint32_t dstHeight = srcHeight >> 1;
//
//		// 0b00(0): Both width and height are even.
//		// 0b01(1): Width is odd, height is even.
//		// 0b10(2): Width is even, height is odd.
//		// 0b11(3): Both width and height are odd.
//		generateMipsCB.SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);
//
//		// How many mipmap levels to compute this pass (max 4 mips per pass)
//		DWORD mipCount;
//
//		// The number of times we can half the size of the texture and get
//		// exactly a 50% reduction in size.
//		// A 1 bit in the width or height indicates an odd dimension.
//		// The case where either the width or the height is exactly 1 is handled
//		// as a special case (as the dimension does not require reduction).
//		_BitScanForward(&mipCount,(dstWidth == 1 ? dstHeight : dstWidth) |
//			(dstHeight == 1 ? dstWidth : dstHeight));
//		// Maximum number of mips to generate is 4.
//		mipCount = std::min<DWORD>(4,mipCount + 1);
//		// Clamp to total number of mips left over.
//		mipCount = (srcMip + mipCount) >= resourceDesc.MipLevels ?
//			resourceDesc.MipLevels - srcMip - 1 : mipCount;
//
//		// Dimensions should not reduce to 0.
//		// This can happen if the width and height are not the same.
//		dstWidth = std::max<DWORD>(1,dstWidth);
//		dstHeight = std::max<DWORD>(1,dstHeight);
//
//		generateMipsCB.SrcMipLevel = srcMip;
//		generateMipsCB.NumMipLevels = mipCount;
//		generateMipsCB.TexelSize.x = 1.0f / static_cast<float>(dstWidth);
//		generateMipsCB.TexelSize.y = 1.0f / static_cast<float>(dstHeight);
//
//		int size = sizeof(generateMipsCB) / sizeof(uint32_t);
//		m_CommandList->SetComputeRoot32BitConstants(GenerateMips::GenerateMipsCB,size,&generateMipsCB,0);
//
//		TransitionBarrier(texture.GetResource(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,srcMip);
//		texture.SetView(srvDesc);
//
//		m_CommandList->SetGraphicsRootDescriptorTable(
//			GenerateMips::SrcMip,
//			GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetGpuHandle(texture.GetHandle(ViewType::SRV).heapOffset));
//
//
//		for (uint32_t mip = 0; mip < mipCount; ++mip)
//		{
//			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
//			uavDesc.Format = resourceDesc.Format;
//			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
//			uavDesc.Texture2D.MipSlice = srcMip + mip + 1;
//
//			TransitionBarrier(texture.GetResource(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,srcMip + mip + 1);
//			texture.SetView(uavDesc);
//
//			m_CommandList->SetGraphicsRootDescriptorTable(
//				GenerateMips::OutMip,
//				GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetGpuHandle(texture.GetHandle(ViewType::UAV).heapOffset));
//
//			//SetUnorderedAccessView(GenerateMips::OutMip,mip,texture,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,srcMip + mip + 1,1,&uavDesc);
//		}
//
//		// Pad any unused mip levels with a default UAV. Doing this keeps the DX12 runtime happy.
//		if (mipCount < 4)
//		{
//			//m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(GenerateMips::OutMip,mipCount,4 - mipCount,m_GenerateMipsPSO->GetDefaultUAV());
//		}
//
//		FlushResourceBarriers();
//		m_CommandList->Dispatch(DivideByMultiple(dstWidth,8),DivideByMultiple(dstHeight,8),1);
//
//		auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());
//		m_ResourceStateTracker->ResourceBarrier(barrier);
//
//		srcMip += mipCount;
//	}
//}


void CommandList::TransitionBarrier(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES stateAfter, UINT subresource, bool flushBarriers)
{
	OPTICK_GPU_EVENT("TransitionBarrier");
	if (resource)
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, stateAfter, subresource);
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

void CommandList::TransitionBarrier(GpuResource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subresource, bool flushBarriers)
{
	OPTICK_EVENT();
	TransitionBarrier(resource.GetResource(), stateAfter, subresource, flushBarriers);
	resource.m_TransitioningState = stateAfter;
}

void CommandList::SetDescriptorTable(unsigned slot, Texture* texture)
{
	OPTICK_EVENT();
	const size_t offset = texture->GetHandle(ViewType::SRV).heapOffset;

	if (offset == -1)
	{
		Logger::Warn("Texture has no heap offset: " + texture->GetName());
		return;
	}

	TransitionBarrier(texture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_CommandList->SetGraphicsRootDescriptorTable(
		slot,
		GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetGpuHandle(offset));


	TrackResource(texture->GetResource());
}

void CommandList::SetRenderTargets(unsigned numberOfTargets, Texture* renderTargets, Texture* depthBuffer)
{

	OPTICK_GPU_EVENT("SetRenderTargets");
	assert(numberOfTargets <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	D3D12_CPU_DESCRIPTOR_HANDLE RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	if (renderTargets)
	{
		for (size_t i = 0; i < numberOfTargets; i++)
		{
			TransitionBarrier(renderTargets[i], D3D12_RESOURCE_STATE_RENDER_TARGET);
			RTVs[i] = renderTargets[i].GetHandle(ViewType::RTV).cpuPtr;
		}
	}
	if (depthBuffer)
	{
		const auto dsv = depthBuffer->GetHandle(ViewType::DSV).cpuPtr;
		TransitionBarrier(*depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_CommandList->OMSetRenderTargets(numberOfTargets, RTVs, FALSE, &dsv);
	}
	else
	{
		m_CommandList->OMSetRenderTargets(numberOfTargets, RTVs, FALSE, nullptr);
	}
}

void CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object)
{
	OPTICK_EVENT();
	m_TrackedObjects.push_back(object);
}

void CommandList::TrackResource(const GpuResource& res)
{
	OPTICK_EVENT();
	TrackResource(res.GetResource());
}

void CommandList::ReleaseTrackedObjects()
{
	OPTICK_EVENT();
	m_TrackedObjects.clear();
}

void CommandList::FlushResourceBarriers()
{
	m_ResourceStateTracker->FlushResourceBarriers(*this);
}

bool CommandList::Close(CommandList& pendingCommandList)
{
	OPTICK_EVENT();
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
	Helpers::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	m_ResourceStateTracker->Reset();

	ReleaseTrackedObjects();
}