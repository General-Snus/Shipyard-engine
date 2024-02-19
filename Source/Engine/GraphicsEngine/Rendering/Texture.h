#pragma once
#include <array> 
#include <memory>
#include <stdexcept>
#include <string>
#include <wrl.h>

using namespace Microsoft::WRL;

#include <Engine/GraphicsEngine/InterOp/GpuResource.h>



class Texture : public GpuResource
{
	friend class GPU;
	friend class TextureHolder;

public:
	Texture() = default;
	~Texture() = default;

	FORCEINLINE const std::wstring& GetName() const { return myName; }
	FORCEINLINE bool IsValid() const { return m_pResource != nullptr; }
	// The name of this texture, for easy ID.
	std::wstring myName;

	virtual void Destroy() override
	{
		GpuResource::Destroy();
		m_hCpuDescriptorHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	bool AllocateTexture(const unsigned width,const unsigned height);

	bool CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB);
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_hCpuDescriptorHandle; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	uint32_t GetDepth() const { return m_Depth; }

protected:
	ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuDescriptorHandle;
};