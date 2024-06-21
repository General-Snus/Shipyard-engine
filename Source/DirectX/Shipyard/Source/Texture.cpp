#include "DirectX/DirectXHeader.pch.h"

#include "DirectX/Shipyard/Texture.h" 
#include <DirectX/DirectXTex/DirectXTex.h> 
#include <DirectX/XTK/ResourceUploadBatch.h> 
#include "DirectX/Shipyard/CommandQueue.h"
#include "DirectX/Shipyard/eDescriptors.h"
#include "DirectX/Shipyard/GPU.h" 


Texture::Texture() : m_Viewport(0,0,0,0,0,1),m_Rect(0,0,0,0)
{
	auto value = HeapHandle(D3D12_CPU_DESCRIPTOR_HANDLE(),D3D12_GPU_DESCRIPTOR_HANDLE(),-1);
	m_DescriptorHandles.emplace(ViewType::SRV,value);
};


bool Texture::AllocateTexture(const Vector2ui dimentions,const std::filesystem::path& name,DXGI_FORMAT Format,D3D12_RESOURCE_FLAGS flags,D3D12_RESOURCE_STATES targetResourceState)
{
	OPTICK_EVENT();
	const auto width = dimentions.x;
	const auto height = dimentions.y;

	m_Rect = D3D12_RECT(0,0,static_cast<LONG>(width),height);
	m_Viewport = D3D12_VIEWPORT(0,0,static_cast<FLOAT>(width),static_cast<FLOAT>(height),0,1);

	myName = name.string();
	m_Format = Format;

	D3D12_RESOURCE_DESC txtDesc = {};
	txtDesc.MipLevels = txtDesc.DepthOrArraySize = 1;
	txtDesc.Format = Format;
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	txtDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	txtDesc.Flags = flags;

	const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

	m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	CD3DX12_CLEAR_VALUE clearValue = {};
	clearValue.Format = Format;

	Helpers::ThrowIfFailed(
		GPU::m_Device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&txtDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			&clearValue,
			IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())));
	m_Resource->SetName(name.wstring().c_str());

	for (int i = 0; i < 4; i++)
	{
		m_ClearColor[i] = clearValue.Color[i];
	}

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
		targetResourceState);

	auto uploadResourcesFinished = resourceUpload.End(GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue().Get());

	uploadResourcesFinished.wait();
	if (!uploadResourcesFinished.valid())
	{
		throw std::runtime_error("Failed to upload texture");
	}

	CheckFeatureSupport();
	if (targetResourceState == D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		SetView(ViewType::RTV);
	}

	return false;
}

void Texture::SetView(D3D12_SHADER_RESOURCE_VIEW_DESC view)
{
	OPTICK_EVENT();
	if (!m_Resource ||
		m_DescriptorHandles.contains(ViewType::SRV) && m_DescriptorHandles.at(ViewType::SRV).heapOffset != -1)
	{
		return;
	}

	HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
	GPU::m_Device->CreateShaderResourceView(m_Resource.Get(),&view,handle.cpuPtr);
	m_DescriptorHandles[ViewType::SRV] = handle;
}

void Texture::SetView(D3D12_UNORDERED_ACCESS_VIEW_DESC view)
{
	view;
}


void Texture::SetView(ViewType view)
{
	OPTICK_EVENT();
	if (!m_Resource ||
		m_DescriptorHandles.contains(view) && m_DescriptorHandles.at(view).heapOffset != -1)
	{
		return;
	}

	auto device = GPU::m_Device;
	CheckFeatureSupport();
	CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

	switch (view)
	{
	case ViewType::UAV:
	{
		HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
		CreateUnorderedAccessView(GPU::m_Device.Get(),m_Resource.Get(),handle.cpuPtr,desc.MipLevels);
		m_DescriptorHandles[ViewType::UAV] = handle;
	}
	break;
	case ViewType::SRV:
	{
		if (CheckDSVSupport())
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = Helpers::GetDepthFormat(m_Format);
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRVDesc.Texture2D.MipLevels = 1;

			HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
			GPU::m_Device->CreateShaderResourceView(m_Resource.Get(),&SRVDesc,handle.cpuPtr);
			m_DescriptorHandles[ViewType::SRV] = handle;
		}
		else
		{
			HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
			CreateShaderResourceView(GPU::m_Device.Get(),m_Resource.Get(),handle.cpuPtr,isCubeMap);
			m_DescriptorHandles[ViewType::SRV] = handle;
		}
		break;
	}
	case ViewType::RTV:
	{
		HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_RTV);
		device->CreateRenderTargetView(m_Resource.Get(),nullptr,handle.cpuPtr);
		m_DescriptorHandles[ViewType::RTV] = handle;
		break;
	}
	case ViewType::DSV:
	{
		HeapHandle handle = GPU::GetHeapHandle(eHeapTypes::HEAP_TYPE_DSV);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_Format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_Resource.Get(),&dsvDesc,handle.cpuPtr);
		m_DescriptorHandles[ViewType::DSV] = handle;
		break;
	}
	default:
		assert(false && "Out of range view type");
		break;
	}
	m_RecentBoundType = view;
}
