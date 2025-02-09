#include "DirectXHeader.pch.h"
#include "../IndexBuffer.h"


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
