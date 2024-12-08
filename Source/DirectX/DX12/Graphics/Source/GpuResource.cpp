#include "DirectXHeader.pch.h"

#include "Graphics/GpuResource.h"
#include "Graphics/GPU.h"

VertexResource::VertexResource(std::filesystem::path name): m_NumVertices(0), m_VertexStride(0)
{
	m_ResourceName = name;
}

void VertexResource::CreateView(size_t numElements, size_t elementSize)
{
	OPTICK_EVENT();
	m_NumVertices = static_cast<uint32_t>(numElements);
	m_VertexStride = static_cast<uint32_t>(elementSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Buffer.NumElements = (m_NumVertices * m_VertexStride) / 4;
	SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
	HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
	GPUInstance.m_Device->CreateShaderResourceView(m_Resource.Get(), &SRVDesc, handle.cpuPtr);
	m_DescriptorHandles[ViewType::SRV] = handle;
}

GpuResource::GpuResource()
	: m_UsageState(D3D12_RESOURCE_STATE_COMMON), m_TransitioningState(static_cast<D3D12_RESOURCE_STATES>(-1)),
	  m_Resource(nullptr), m_FormatSupport()
{
	CheckFeatureSupport();
}

// GpuResource& GpuResource::operator=(const GpuResource& other)
//{
//	if (this != &other)
//	{
//		m_Resource = other.m_Resource;
//		m_FormatSupport = other.m_FormatSupport;
//		m_ResourceName = other.m_ResourceName;
//	}
//	return *this;
// }
//
// GpuResource& GpuResource::operator=(GpuResource&& other) noexcept
//{
//	if (this != &other)
//	{
//		m_Resource = std::move(other.m_Resource);
//		m_FormatSupport = other.m_FormatSupport;
//		m_ResourceName = std::move(other.m_ResourceName);
//		other.Reset();
//	}
//	return *this;
// }

void GpuResource::Destroy()
{
	m_Resource = nullptr;
	for (auto& [type, pair] : m_DescriptorHandles)
	{
		pair.cpuPtr.ptr = 0;
		pair.heapOffset = -1;
	}

	// Reset states
	m_UsageState = D3D12_RESOURCE_STATE_COMMON;
	m_TransitioningState = D3D12_RESOURCE_STATE_COMMON;
	m_RecentBoundType = ViewType::SRV;
	m_Format = DXGI_FORMAT_UNKNOWN;
	m_ResourceName.clear();
}

void GpuResource::CreateView(size_t numElements, size_t elementSize)
{
	OPTICK_GPU_EVENT("CreateView");
	UNREFERENCED_PARAMETER(numElements);
	UNREFERENCED_PARAMETER(elementSize);
}

void GpuResource::Reset()
{
	OPTICK_EVENT();
	m_Resource.Reset();
	m_ResourceName.clear();
	m_FormatSupport = {};
	m_DescriptorHandles.clear();
}

bool GpuResource::CheckSrvSupport() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
}

bool GpuResource::CheckRTVSupport() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
}

bool GpuResource::CheckUAVSupport() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
}

bool GpuResource::CheckDSVSupport() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
}

void GpuResource::SetView(ViewType view)
{
	OPTICK_EVENT();
	if (!m_Resource || m_DescriptorHandles.contains(view) && m_DescriptorHandles.at(view).heapOffset != -1)
	{
		return;
	}

	auto device = GPUInstance.m_Device;
	CheckFeatureSupport();
	CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

	switch (view)
	{
	case ViewType::UAV:
		{
			HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
			CreateUnorderedAccessView(GPUInstance.m_Device.Get(), m_Resource.Get(), handle.cpuPtr, desc.MipLevels);
			m_DescriptorHandles[ViewType::UAV] = handle;
		}
		break;
	case ViewType::SRV:
		{
			if (CheckDSVSupport())
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
				SRVDesc.Format = Helpers::GetDepthFormat(m_Format);
				SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				SRVDesc.Texture2D.MipLevels = 1;

				HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
				GPUInstance.m_Device->CreateShaderResourceView(m_Resource.Get(), &SRVDesc, handle.cpuPtr);
				m_DescriptorHandles[ViewType::SRV] = handle;
			}
			else
			{
				HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
				CreateShaderResourceView(GPUInstance.m_Device.Get(), m_Resource.Get(), handle.cpuPtr);
				m_DescriptorHandles[ViewType::SRV] = handle;
			}
		}
		break;
	case ViewType::RTV:
		{
			HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_RTV);

			device->CreateRenderTargetView(m_Resource.Get(), nullptr, handle.cpuPtr);
			m_DescriptorHandles[ViewType::RTV] = handle;
		}
		break;
	case ViewType::DSV:
		{
			HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_DSV);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = m_Format;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			device->CreateDepthStencilView(m_Resource.Get(), &dsvDesc, handle.cpuPtr);
			m_DescriptorHandles[ViewType::DSV] = handle;
		}
		break;
	default:
		assert(false && "Out of range view type");
		break;
	}
	m_RecentBoundType = view;
}

void GpuResource::SetView(ViewType view, HeapHandle handle)
{
	OPTICK_EVENT();
	if (!m_Resource)
	{
		return;
	}

	auto device = GPUInstance.m_Device;
	CheckFeatureSupport();
	CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

	switch (view)
	{
	case ViewType::UAV:
		{
			CreateUnorderedAccessView(GPUInstance.m_Device.Get(), m_Resource.Get(), handle.cpuPtr, desc.MipLevels);
			m_DescriptorHandles[ViewType::UAV] = handle;
		}
		break;
	case ViewType::SRV:
		{
			if (CheckDSVSupport())
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
				SRVDesc.Format = Helpers::GetDepthFormat(m_Format);
				SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				SRVDesc.Texture2D.MipLevels = 1;

				GPUInstance.m_Device->CreateShaderResourceView(m_Resource.Get(), &SRVDesc, handle.cpuPtr);
				m_DescriptorHandles[ViewType::SRV] = handle;
			}
			else
			{
				CreateShaderResourceView(GPUInstance.m_Device.Get(), m_Resource.Get(), handle.cpuPtr);
				m_DescriptorHandles[ViewType::SRV] = handle;
			}
		}
		break;
	case ViewType::RTV:
		{
			device->CreateRenderTargetView(m_Resource.Get(), nullptr, handle.cpuPtr);
			m_DescriptorHandles[ViewType::RTV] = handle;
		}
		break;
	case ViewType::DSV:
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = m_Format;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			device->CreateDepthStencilView(m_Resource.Get(), &dsvDesc, handle.cpuPtr);
			m_DescriptorHandles[ViewType::DSV] = handle;
		}
		break;
	default:
		assert(false && "Out of range view type");
		break;
	}
	m_RecentBoundType = view;
}

void GpuResource::ClearView(ViewType view)
{
	OPTICK_EVENT();
	m_DescriptorHandles.contains(view) ? m_DescriptorHandles.erase(view) : NULL;
}

HeapHandle GpuResource::GetHandle(ViewType type)
{
	OPTICK_EVENT();
	if (m_DescriptorHandles.contains(type) && m_DescriptorHandles.at(type).heapOffset != -1)
	{
		return m_DescriptorHandles.at(type);
	}
	SetView(type);
	return m_DescriptorHandles.at(type);
}

HeapHandle GpuResource::GetHandle(ViewType type) const
{
	OPTICK_EVENT();
	if (m_DescriptorHandles.contains(type) && m_DescriptorHandles.at(type).heapOffset != -1)
	{
		return m_DescriptorHandles.at(type);
	}
	return HeapHandle();
}

HeapHandle GpuResource::CreateViewWithHandle(ViewType type, HeapHandle handle)
{
	OPTICK_EVENT();
	SetView(type, handle);
	return m_DescriptorHandles.at(type);
}

HeapHandle GpuResource::GetHandle() const
{
	return m_DescriptorHandles.at(m_RecentBoundType);
}

int GpuResource::GetHeapOffset() const
{
	OPTICK_EVENT();
	if (m_DescriptorHandles.at(m_RecentBoundType).heapOffset != -1 &&
		m_DescriptorHandles.at(m_RecentBoundType).heapOffset > GPUInstance.m_HeapSizes[static_cast<int>(
			m_RecentBoundType)])
	{
		throw InternalGPUError("HeapOffset was out of range of heap");
	}
	return m_DescriptorHandles.at(m_RecentBoundType).heapOffset;
};

void GpuResource::SetResource(const ComPtr<ID3D12Resource>& resource)
{
	OPTICK_GPU_EVENT("SetResource");
	m_Resource = resource;
	m_Resource->SetName(m_ResourceName.c_str());
	CheckFeatureSupport();
}

ComPtr<ID3D12Resource> GpuResource::GetResource()
{
	return m_Resource;
}

const ComPtr<ID3D12Resource>& GpuResource::GetResource() const
{
	return m_Resource;
}

ID3D12Resource** GpuResource::GetAddressOf()
{
	return m_Resource.GetAddressOf();
}

bool GpuResource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
{
	return (m_FormatSupport.Support1 & formatSupport) != 0;
}

bool GpuResource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
{
	return (m_FormatSupport.Support2 & formatSupport) != 0;
}

void GpuResource::CheckFeatureSupport()
{
	OPTICK_EVENT();
	if (m_Resource)
	{
		const auto desc = m_Resource->GetDesc();

		m_FormatSupport.Format = desc.Format;
		Helpers::ThrowIfFailed(GPUInstance.m_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
		                                                                 sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
	}
	else
	{
		m_FormatSupport = {};
	}
}

IndexResource::IndexResource() : GpuResource(), m_NumIndices(0), m_IndexFormat(DXGI_FORMAT_UNKNOWN)
{
}

IndexResource::IndexResource(const std::filesystem::path& name)
	: GpuResource(), m_NumIndices(0), m_IndexFormat(DXGI_FORMAT_UNKNOWN)
{
	m_ResourceName = name;
}

void IndexResource::CreateView(size_t numElements, size_t elementSize)
{
	assert(elementSize == 2 || elementSize == 4 && "Indices must be 16, or 32-bit integers.");

	m_NumIndices = static_cast<uint32_t>(numElements);
	m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
	m_IndexBufferView.SizeInBytes = static_cast<UINT>(numElements * elementSize);
	m_IndexBufferView.Format = m_IndexFormat;
}
