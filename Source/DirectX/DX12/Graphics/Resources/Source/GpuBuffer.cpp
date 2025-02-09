#include "DirectXHeader.pch.h"
#include "../GpuBuffer.h"
#include <DirectX/DX12/Graphics/GPU.h>


GpuBuffer::GpuBuffer(const BufferDescription& desc, void* intialData) : desc(desc)
{
	uint64_t buffer_size = desc.size;
	if (desc.misc_flags || BufferDescription::BufferFlag::ConstantBuffer)
		buffer_size = Math::Align(buffer_size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	D3D12_RESOURCE_DESC resource_desc{};
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	resource_desc.Width = buffer_size;
	resource_desc.Height = 1;
	resource_desc.MipLevels = 1;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.Alignment = 0;
	resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;

	if (desc.bind_flags && BufferDescription::BufferBinds::UnorderedAccess)
		resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	if (desc.bind_flags && BufferDescription::BufferBinds::ShaderResource)
		resource_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_COMMON; 

	CD3DX12_HEAP_DESC heap = CD3DX12_HEAP_DESC();
	if (desc.resource_usage == BufferDescription::ResourceUsage::Readback)
	{
		heap.Properties.Type = D3D12_HEAP_TYPE_READBACK;
		resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
		resource_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	}
	else if (desc.resource_usage == BufferDescription::ResourceUsage::Upload)
	{
		heap.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	  

	auto& device = GPUInstance.m_Device;
	HRESULT hr = device->CreateCommittedResource(
		&heap.Properties,
		heap.Flags,
		&resource_desc,
		resource_state,
		nullptr,
		IID_PPV_ARGS(m_Resource.GetAddressOf())
	);

	if (FAILED(hr))
	{
		throw hr;
		return;
	}


	if (desc.resource_usage == BufferDescription::ResourceUsage::Readback)
	{
		hr = m_Resource->Map(0, nullptr, &mapped_data);
 	}
	else if (desc.resource_usage == BufferDescription::ResourceUsage::Upload)
	{
		D3D12_RANGE read_range{};
		hr = m_Resource->Map(0, &read_range, &mapped_data);
 
		if (intialData)
		{
			memcpy(mapped_data, intialData, desc.size);
		}
	}

	if (intialData != nullptr && desc.resource_usage != BufferDescription::ResourceUsage::Upload)
	{
		const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto       commandList = commandQueue->GetCommandList(L"GpuBufferInitialization");

		commandList->CopyBuffer(*this,1, sizeof(desc.size), mapped_data,D3D12_RESOURCE_FLAG_NONE, CD3DX12_HEAP_PROPERTIES(heap.Properties));
 		if (desc.bind_flags || BufferDescription::BufferBinds::ShaderResource)
		{
			commandList->TransitionBarrier(*this, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE); 
		}
		commandQueue->ExecuteCommandList(commandList);
	}
}

bool GpuBuffer::IsMapped() const
{
	return mapped_data != nullptr;
}

inline void* GpuBuffer::Map()
{
	if (mapped_data) return mapped_data;

	HRESULT hr;
	if (desc.resource_usage == BufferDescription::ResourceUsage::Readback)
	{
		hr = m_Resource->Map(0, nullptr, &mapped_data);
		if (FAILED(hr)) { return nullptr; }
	}
	else if (desc.resource_usage == BufferDescription::ResourceUsage::Upload)
	{
		D3D12_RANGE read_range{};
		hr = m_Resource->Map(0, &read_range, &mapped_data); 
		if (FAILED(hr)) { return nullptr; } 
	}
	return mapped_data;
}

void GpuBuffer::Unmap()
{
	m_Resource->Unmap(0, nullptr);
	mapped_data = nullptr;
}

void GpuBuffer::Update(void const* src_data, uint64_t data_size, uint64_t offset)
{
	src_data; data_size; offset;
	if (mapped_data)
	{
		memcpy((uint8_t*) mapped_data+ offset, src_data, data_size);
	}
	else
	{
		Map();
 		memcpy((uint8_t*)mapped_data + offset, src_data, data_size);
	}
}

void* GpuBuffer::GetMappedData() const
{
	return mapped_data;
}

BufferDescription GpuBuffer::GetDesc() const
{
	return desc;
}


uint64_t GpuBuffer::GetSize() const
{
	return desc.size;
}
