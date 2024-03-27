#include "DirectXHeader.pch.h"

#include "Shipyard/Texture.h"

#include <DirectXTex/DirectXTex.h>
#include <XTK/ResourceUploadBatch.h>

#include "Shipyard/CommandQueue.h"
#include "Shipyard/eDescriptors.h"
#include "Shipyard/GPU.h"
#include "Shipyard/Helpers.h"


Texture::Texture() : m_Width(0),m_Height(0)
{
	auto value = OffsetHandlePair(D3D12_CPU_DESCRIPTOR_HANDLE(),-1);
	m_DescriptorHandles.emplace(ViewType::SRV,value);
};

void Texture::Destroy()

{
	GpuResource::Destroy();
	for (auto& [type,pair] : m_DescriptorHandles)
	{
		pair.first.ptr = 0;
		pair.second = -1;
	}
}

bool Texture::AllocateTexture(const Vector2ui dimentions,const std::filesystem::path& name,DXGI_FORMAT Format,D3D12_RESOURCE_FLAGS flags)
{
	m_Width = dimentions.x;
	m_Height = dimentions.y;
	myName = name.string();

	D3D12_RESOURCE_DESC txtDesc = {};
	txtDesc.MipLevels = txtDesc.DepthOrArraySize = 1;
	txtDesc.Format = Format;
	txtDesc.Width = m_Width;
	txtDesc.Height = m_Height;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	txtDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	txtDesc.Flags = flags;

	CD3DX12_HEAP_PROPERTIES heapProps;
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	CD3DX12_CLEAR_VALUE   clearValue
	(
		Format,
		&m_ClearColor.x
	);
	Helpers::ThrowIfFailed(
		GPU::m_Device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&txtDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			&clearValue,
			IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())));
	m_Resource->SetName(name.wstring().c_str());

	const auto pixelSize = BitsPerPixel(Format);
	static const uint32_t s_whitePixels = 0xFFFFFFFF;

	void* pixelData = malloc(txtDesc.Height * txtDesc.Width * pixelSize);
	memset(pixelData,s_whitePixels,txtDesc.Height * txtDesc.Width * pixelSize);
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pixelData;
	textureData.RowPitch = txtDesc.Width * pixelSize;
	textureData.SlicePitch = txtDesc.Height * txtDesc.Width * pixelSize;

	ResourceUploadBatch resourceUpload(GPU::m_Device.Get());
	resourceUpload.Begin();
	resourceUpload.Upload(m_Resource.Get(),0,&textureData,1);
	resourceUpload.Transition(
		m_Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto uploadResourcesFinished = resourceUpload.End(GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue().Get());

	uploadResourcesFinished.wait();
	if (!uploadResourcesFinished.valid())
	{
		throw std::runtime_error("Failed to upload texture");
	}

	CheckFeatureSupport();
	SetView(ViewType::RTV);

	return false;
}

void Texture::SetView(ViewType view)
{
	if (!m_Resource ||
		m_DescriptorHandles.contains(view) && m_DescriptorHandles.at(view).second != -1)
	{
		return;
	}

	auto device = GPU::m_Device;
	CheckFeatureSupport();
	CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

	switch (view)
	{
	case ViewType::SRV:
	{
		assert(CheckSRVSupport());
		const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->Allocate();
		const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetCpuHandle(heapOffset);



		CreateShaderResourceView(GPU::m_Device.Get(),m_Resource.Get(),descriptorHandle,isCubeMap);
		m_DescriptorHandles[ViewType::SRV] = OffsetHandlePair(descriptorHandle,heapOffset);

		break;
	}
	case ViewType::RTV:
	{
		assert(CheckRTVSupport());
		const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->Allocate();
		const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->GetCpuHandle(heapOffset);
		device->CreateRenderTargetView(m_Resource.Get(),nullptr,descriptorHandle);
		m_DescriptorHandles[ViewType::RTV] = OffsetHandlePair(descriptorHandle,heapOffset);
		break;
	}
	case ViewType::UAV:
		break;
	case ViewType::DSV:
	{
		assert(CheckDSVSupport());
		const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->Allocate();
		const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->GetCpuHandle(heapOffset);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_Resource.Get(),&dsvDesc,descriptorHandle);
		m_DescriptorHandles[ViewType::DSV] = OffsetHandlePair(descriptorHandle,heapOffset);
		break;
	}
	default:
		assert(false && "Out of range view type");
		break;
	}
	m_RecentBoundType = view;
}

bool Texture::CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB)
{
	filePtr; fileSize; sRGB;
	/*if (m_hCpuDescriptorHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		m_hCpuDescriptorHandle = GPU::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	HRESULT hr = CreateDDSTextureFromMemory(GPU::m_Device.Get(),
		(const uint8_t*)filePtr,fileSize,0,sRGB,&m_Resource,m_hCpuDescriptorHandle);*/

	D3D12_DESCRIPTOR_RANGE texture2DRange{};
	texture2DRange.BaseShaderRegister = 0;
	texture2DRange.NumDescriptors = 50;
	texture2DRange.OffsetInDescriptorsFromTableStart = 0;
	texture2DRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texture2DRange.RegisterSpace = 1;

	return SUCCEEDED(true);
}
OffsetHandlePair Texture::GetHandle(ViewType type)
{
	if (m_DescriptorHandles.find(type) != m_DescriptorHandles.end())
	{
		return  m_DescriptorHandles.at(type);
	}
	SetView(type);
	return  m_DescriptorHandles.at(type);
}

OffsetHandlePair Texture::GetHandle() const
{
	return  m_DescriptorHandles.at(m_RecentBoundType);
}

int Texture::GetHeapOffset() const
{
	return m_DescriptorHandles.at(m_RecentBoundType).second;
};