#include "DirectXHeader.pch.h"
#include "../VertexBuffer.h" 
#include <DirectX/DX12/Graphics/GPU.h>


VertexResource::VertexResource(std::filesystem::path name) : m_NumVertices(0), m_VertexStride(0)
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