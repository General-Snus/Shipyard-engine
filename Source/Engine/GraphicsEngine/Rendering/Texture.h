#pragma once
#include <array> 
#include <Directx/XTK/DescriptorHeap.h>
#include <Engine/GraphicsEngine/InterOp/eDescriptors.h>
#include <Engine/GraphicsEngine/InterOp/GpuResource.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <wrl.h>

namespace DirectX
{
	class DescriptorHeap;
}

using namespace Microsoft::WRL;




class Texture : public GpuResource
{
	friend class GPU;
	friend class TextureHolder;

public:
	Texture() { m_DescriptorHandle.ptr = 0; };
	~Texture() = default;
	void Initialize();
	FORCEINLINE const std::wstring& GetName() const { return myName; }
	FORCEINLINE bool IsValid() const { return m_pResource != nullptr; }
	// The name of this texture, for easy ID.
	std::wstring myName;

	virtual void Destroy() override
	{
		GpuResource::Destroy();
		m_DescriptorHandle.ptr = 0;
	}

	bool AllocateTexture(const unsigned width,const unsigned height);

	bool CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB);
	D3D12_CPU_DESCRIPTOR_HANDLE  GetSRV() const { return  m_DescriptorHandle; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	uint32_t GetDepth() const { return m_Depth; }

protected:
	//std::unique_ptr<DirectX::DescriptorHeap> m_DescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandle;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;
};