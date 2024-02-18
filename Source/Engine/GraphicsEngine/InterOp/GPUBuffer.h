#pragma once
#include <string> 
#include "GpuResource.h"

class GPUBuffer : public GpuResource
{
public:
	void Create(const std::wstring& name,uint32_t NumElements,uint32_t ElementSize,
		const void* initialData = nullptr);

protected:
	GPUBuffer(void) : m_BufferSize(0),m_ElementCount(0),m_ElementSize(0)
	{
		m_ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		m_UAV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_SRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}
private:

	D3D12_CPU_DESCRIPTOR_HANDLE m_UAV; // Unordered Access View
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRV; // Shader Resource View

	size_t m_BufferSize;
	uint32_t m_ElementCount;
	uint32_t m_ElementSize;
	D3D12_RESOURCE_FLAGS m_ResourceFlags;

};

