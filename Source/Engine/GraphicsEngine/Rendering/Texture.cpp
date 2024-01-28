#include <InterOp/XTK/DDSTextureLoader.h>
#include "GraphicsEngine.pch.h"
#include "Texture.h"


bool Texture::CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB)
{
	if (m_hCpuDescriptorHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		m_hCpuDescriptorHandle = GPU::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	HRESULT hr = CreateDDSTextureFromMemory(Graphics::g_Device,
		(const uint8_t*)filePtr,fileSize,0,sRGB,&m_pResource,m_hCpuDescriptorHandle);

	return SUCCEEDED(hr);
}