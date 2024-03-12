#pragma once
#include <DirectX/Shipyard/GpuResource.h> 


using namespace Microsoft::WRL;

class Texture : public GpuResource
{
	friend class GPU;
	friend class TextureHolder;

public:
	explicit Texture() : m_Width(0),m_Height(0),m_Depth(0) { m_DescriptorHandle.ptr = 0; };
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
	void CreateView();
	bool CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB);
	D3D12_CPU_DESCRIPTOR_HANDLE  GetSRV() const { return  m_DescriptorHandle; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	uint32_t GetDepth() const { return m_Depth; }

	virtual bool IsSRV() const override { return true; };



	bool CheckSRVSupport()
	{
		return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
	}

	bool CheckRTVSupport()
	{
		return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
	}

	bool CheckUAVSupport()
	{
		return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
			CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
			CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
	}

	bool CheckDSVSupport()
	{
		return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
	}

protected:
	//std::unique_ptr<DirectX::DescriptorHeap> m_DescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandle;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;
};