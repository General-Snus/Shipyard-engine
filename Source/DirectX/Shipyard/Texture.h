#pragma once
#include <DirectX/Shipyard/GpuResource.h> 
#include <unordered_map> 


using namespace Microsoft::WRL;

enum class ViewType
{
	SRV,
	RTV,
	UAV,
	DSV
};

using OffsetHandlePair = std::pair<D3D12_CPU_DESCRIPTOR_HANDLE,int>;

class Texture : public GpuResource
{
	friend class GPU;
	friend class TextureHolder;

public:
	explicit Texture();

	FORCEINLINE const std::string& GetName() const { return myName; }
	FORCEINLINE bool IsValid() const { return m_pResource != nullptr; }
	// The name of this texture, for easy ID.
	std::string myName;

	void Destroy() override;
	//Default state is render target
	bool AllocateTexture(const unsigned width,const unsigned height,const std::filesystem::path& name = "Unnamed texture");
	void SetView(ViewType view);
	bool CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB);

	OffsetHandlePair  GetHandle(ViewType type);

	OffsetHandlePair  GetHandle() const;

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	//-1 is invalid sizet so we need to check that, flinging in max sizet will cause crash making sure we check the value by instantcrashing in case we dont
	int GetHeapOffset() const;

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
	//D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptorHandle;
	//D3D12_CPU_DESCRIPTOR_HANDLE m_SRVDescriptorHandle; 
	//int heapOffset = -1;

	std::unordered_map<ViewType,OffsetHandlePair> m_DescriptorHandles;
	ViewType m_RecentBoundType = ViewType::SRV;
	Vector4f m_ClearColor = { 0,0,0,1 };
	uint32_t m_Width;
	uint32_t m_Height;
};