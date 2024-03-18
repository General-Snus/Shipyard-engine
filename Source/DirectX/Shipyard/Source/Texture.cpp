#include "DirectXHeader.pch.h"

#include "Shipyard/Texture.h"

#include <XTK/ResourceUploadBatch.h>

#include "Shipyard/CommandQueue.h"
#include "Shipyard/eDescriptors.h"
#include "Shipyard/GPU.h"
#include "Shipyard/Helpers.h"

void Texture::Initialize()
{
	//m_DescriptorHandle = std::make_unique<DescriptorHeap>(GPU::m_Device.Get(),eDescriptors::Textures);
}

bool Texture::AllocateTexture(const unsigned width,const unsigned height,const std::filesystem::path& name)
{
	m_Width = width;
	m_Height = height;
	Initialize();
	myName = name;
	// This creates a x y texture in RGBA format
	D3D12_RESOURCE_DESC txtDesc = {};
	txtDesc.MipLevels = txtDesc.DepthOrArraySize = 1;
	txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	txtDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	txtDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	CD3DX12_HEAP_PROPERTIES heapProps;
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	Helpers::ThrowIfFailed(
		GPU::m_Device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&txtDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())));
	m_pResource->SetName(name.wstring().c_str());

	static const uint32_t s_whitePixels = 0xFFFFFFFF;
	void* pixelData = malloc(txtDesc.Height * txtDesc.Width * 4);
	memset(pixelData,s_whitePixels,txtDesc.Height * txtDesc.Width * 4);

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pixelData;
	textureData.RowPitch = txtDesc.Width * 4;
	textureData.SlicePitch = txtDesc.Height * txtDesc.Width * 4;

	//m_DescriptorHandle = std::make_unique<DescriptorHeap>(GPU::m_Device.Get(),eDescriptors::Count);


	ResourceUploadBatch resourceUpload(GPU::m_Device.Get());

	resourceUpload.Begin();

	resourceUpload.Upload(m_pResource.Get(),0,&textureData,1);

	resourceUpload.Transition(
		m_pResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto uploadResourcesFinished = resourceUpload.End(GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue().Get());

	uploadResourcesFinished.wait();
	if (!uploadResourcesFinished.valid())
	{
		throw std::runtime_error("Failed to upload texture");
	}
	CheckFeatureSupport();
	CreateView(); 

	return false;
}

void Texture::CreateView()
{
	if (m_pResource)
	{
		auto device = GPU::m_Device;

		CD3DX12_RESOURCE_DESC desc(m_pResource->GetDesc());

		if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 &&
			CheckRTVSupport())
		{
			heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->Allocate();
			m_DescriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->GetCpuHandle(heapOffset);
			device->CreateRenderTargetView(m_pResource.Get(),nullptr,m_DescriptorHandle);
		}
		else if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 &&
			CheckDSVSupport())
		{
			heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->Allocate();
			m_DescriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->GetCpuHandle(heapOffset);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format = DXGI_FORMAT_D32_FLOAT;
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D.MipSlice = 0;
			dsv.Flags = D3D12_DSV_FLAG_NONE;

			device->CreateDepthStencilView(m_pResource.Get(),&dsv,m_DescriptorHandle);
		} 

		else if (CheckSRVSupport())
		{
			heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->Allocate();
			m_DescriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetCpuHandle(heapOffset);
			CreateShaderResourceView(GPU::m_Device.Get(),m_pResource.Get(),
				m_DescriptorHandle);
		}
	}
}

bool Texture::CreateDDSFromMemory(const void* filePtr,size_t fileSize,bool sRGB)
{
	filePtr; fileSize; sRGB;
	/*if (m_hCpuDescriptorHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		m_hCpuDescriptorHandle = GPU::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	HRESULT hr = CreateDDSTextureFromMemory(GPU::m_Device.Get(),
		(const uint8_t*)filePtr,fileSize,0,sRGB,&m_pResource,m_hCpuDescriptorHandle);*/

	D3D12_DESCRIPTOR_RANGE texture2DRange{};
	texture2DRange.BaseShaderRegister = 0;
	texture2DRange.NumDescriptors = 50;
	texture2DRange.OffsetInDescriptorsFromTableStart = 0;
	texture2DRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texture2DRange.RegisterSpace = 1;

	return SUCCEEDED(true);
};