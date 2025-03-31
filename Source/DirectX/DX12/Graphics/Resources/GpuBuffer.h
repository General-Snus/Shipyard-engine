#pragma once
#include <DirectX/DX12/Graphics/GpuResource.h>

struct BufferDescription { 
	enum class BufferBinds : unsigned {
		None = 0,
		ShaderResource = 1 << 0,
		UnorderedAccess = 1 << 1,
		Count = 1 << 2
	};

	enum class ResourceUsage : unsigned {
		Default = 0,
		Upload = 1 << 0, 
		Readback = 1 << 1,
		Count = 1 << 2
	};

	enum class BufferFlag : unsigned {
		None = 0,
		ConstantBuffer = 1 << 0,
		Count = 1 << 1
	};


	uint64_t size = 0;
	ResourceUsage resource_usage = ResourceUsage::Default;
	BufferBinds bind_flags = BufferBinds::None;
	BufferFlag misc_flags = BufferFlag::None;
	uint32_t stride = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};
 
ENABLE_ENUM_BITWISE_OPERATORS(BufferDescription::BufferBinds)
ENABLE_ENUM_BITWISE_OPERATORS(BufferDescription::ResourceUsage)
ENABLE_ENUM_BITWISE_OPERATORS(BufferDescription::BufferFlag)

class GpuBuffer : public GpuResource
{
public:
	explicit GpuBuffer(const BufferDescription& desc, void* intialData);

	BufferDescription GetDesc() const;
	uint64_t GetSize() const;

	bool IsMapped() const;
	void* GetMappedData() const;
	void* Map();
	void Unmap();
	void Update(void const* src_data, uint64_t data_size, uint64_t offset = 0);

	template<typename T>
	T* GetMappedData() const;

	template<typename T>
	void Update(T const& src_data);
private:
	void* mapped_data = nullptr;
	BufferDescription desc;
};
 

template<typename T>
T* GpuBuffer::GetMappedData() const
{
	return reinterpret_cast<T*>(mapped_data);
}
template<typename T>
void GpuBuffer::Update(T const& src_data)
{
	Update(&src_data, sizeof(T));
} 
