#include "DirectXHeader.pch.h"

#include "Shipyard/GpuResource.h"

#include <cassert>

#include "Shipyard/GPU.h"

VertexResource::VertexResource(std::wstring name)
{
	m_ResourceName = name;
}

void VertexResource::CreateView(size_t numElements,size_t elementSize)
{
	m_NumVertices = (uint32_t)numElements;
	m_VertexStride = (uint32_t)elementSize;

	m_VertexBufferView.BufferLocation = m_pResource->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_NumVertices * m_VertexStride);
	m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_VertexStride);
}

GpuResource::GpuResource() : m_UsageState(D3D12_RESOURCE_STATE_COMMON),
m_TransitioningState((D3D12_RESOURCE_STATES)-1),
m_pResource(nullptr),m_FormatSupport()
{
}

GpuResource::GpuResource(const GpuResource& toCopy) :
	m_UsageState(toCopy.m_UsageState),
	m_TransitioningState(toCopy.m_TransitioningState),
	m_FormatSupport(toCopy.m_FormatSupport),
	m_pResource(toCopy.m_pResource)
{
}

GpuResource& GpuResource::operator=(const GpuResource& other)
{
	if (this != &other)
	{
		m_pResource = other.m_pResource;
		m_FormatSupport = other.m_FormatSupport;
		m_ResourceName = other.m_ResourceName;
	}
	return *this;
}

GpuResource& GpuResource::operator=(GpuResource&& other) noexcept
{
	if (this != &other)
	{
		m_pResource = std::move(other.m_pResource);
		m_FormatSupport = other.m_FormatSupport;
		m_ResourceName = std::move(other.m_ResourceName);
		other.Reset();
	}
	return *this;
}

void GpuResource::CreateView(size_t numElements,size_t elementSize)
{
	UNREFERENCED_PARAMETER(numElements);
	UNREFERENCED_PARAMETER(elementSize);
}

void GpuResource::Reset()
{
	m_pResource.Reset();
	m_ResourceName.clear();
	m_FormatSupport = {};
}


ID3D12Resource* GpuResource::operator->()
{
	return m_pResource.Get();
}

const ID3D12Resource* GpuResource::operator->() const
{
	return m_pResource.Get();
}

void GpuResource::SetResource(const ComPtr<ID3D12Resource>& resource)
{
	m_pResource = resource;
	m_pResource->SetName(m_ResourceName.c_str());
}

ComPtr<ID3D12Resource> GpuResource::GetResource()
{
	return m_pResource;
}

const ComPtr<ID3D12Resource>& GpuResource::GetResource() const
{
	return m_pResource;
}

ID3D12Resource** GpuResource::GetAddressOf()
{
	return m_pResource.GetAddressOf();
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
	if (m_pResource)
	{
		auto desc = m_pResource->GetDesc();

		m_FormatSupport.Format = desc.Format;
		Helpers::ThrowIfFailed(GPU::m_Device->CheckFeatureSupport(
			D3D12_FEATURE_FORMAT_SUPPORT,
			&m_FormatSupport,
			sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
	}
	else
	{
		m_FormatSupport = {};
	}
}

IndexResource::IndexResource(std::wstring name)
{
	m_ResourceName = name;
}

void IndexResource::CreateView(size_t numElements,size_t elementSize)
{
	assert(elementSize == 2 || elementSize == 4 && "Indices must be 16, or 32-bit integers.");

	m_NumIndices = (uint32_t)numElements;
	m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_IndexBufferView.BufferLocation = m_pResource->GetGPUVirtualAddress();
	m_IndexBufferView.SizeInBytes = static_cast<UINT>(numElements * elementSize);
	m_IndexBufferView.Format = m_IndexFormat;
}
