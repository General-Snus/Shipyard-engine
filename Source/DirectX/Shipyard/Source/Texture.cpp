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
	auto value = textureHandle(D3D12_CPU_DESCRIPTOR_HANDLE(),-1,0);
	m_DescriptorHandles.emplace(ViewType::SRV,value);
};

void Texture::Destroy()

{
	GpuResource::Destroy();
	for (auto& [type,pair] : m_DescriptorHandles)
	{
		pair.cpuPtr.ptr = 0;
		pair.heapOffset = -1;
		pair.space = 0;
	}
}

bool Texture::AllocateTexture(const Vector2ui dimentions,const std::filesystem::path& name,DXGI_FORMAT Format,D3D12_RESOURCE_FLAGS flags,D3D12_RESOURCE_STATES targetResourceState)
{
	m_Width = dimentions.x;
	m_Height = dimentions.y;
	myName = name.string();
	m_Format = Format;

	D3D12_RESOURCE_DESC txtDesc = {};
	txtDesc.MipLevels = txtDesc.DepthOrArraySize = 1;
	txtDesc.Format = Format;
	txtDesc.Width = m_Width;
	txtDesc.Height = m_Height;
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

void Texture::SetView(ViewType view,unsigned space)
{
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
	case ViewType::SRV:
	{
		if (CheckDSVSupport())
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = GetDepthFormat(m_Format);
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRVDesc.Texture2D.MipLevels = 1;

			const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->Allocate();
			const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetCpuHandle(heapOffset);
			GPU::m_Device->CreateShaderResourceView(m_Resource.Get(),&SRVDesc,descriptorHandle);
			m_DescriptorHandles[ViewType::SRV] = textureHandle(descriptorHandle,heapOffset,space);
		}
		else
		{

			const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->Allocate();
			const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV]->GetCpuHandle(heapOffset);
			CreateShaderResourceView(GPU::m_Device.Get(),m_Resource.Get(),descriptorHandle,isCubeMap);
			m_DescriptorHandles[ViewType::SRV] = textureHandle(descriptorHandle,heapOffset,space);
		}
		break;
	}
	case ViewType::RTV:
	{
		const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->Allocate();
		const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV]->GetCpuHandle(heapOffset);
		device->CreateRenderTargetView(m_Resource.Get(),nullptr,descriptorHandle);
		m_DescriptorHandles[ViewType::RTV] = textureHandle(descriptorHandle,heapOffset);
		break;
	}
	case ViewType::UAV:
		break;
	case ViewType::DSV:
	{
		const int heapOffset = (int)GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->Allocate();
		const auto descriptorHandle = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV]->GetCpuHandle(heapOffset);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_Format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_Resource.Get(),&dsvDesc,descriptorHandle);
		m_DescriptorHandles[ViewType::DSV] = textureHandle(descriptorHandle,heapOffset);
		break;
	}
	default:
		assert(false && "Out of range view type");
		break;
	}
	m_RecentBoundType = view;
}

void Texture::ClearView(ViewType view)
{
	m_DescriptorHandles.contains(view) ? m_DescriptorHandles.erase(view) : NULL;
}

textureHandle Texture::GetHandle(ViewType type)
{
	if (m_DescriptorHandles.find(type) != m_DescriptorHandles.end() && m_DescriptorHandles.at(type).heapOffset != -1)
	{
		return  m_DescriptorHandles.at(type);
	}
	SetView(type);
	return  m_DescriptorHandles.at(type);
}

textureHandle Texture::GetHandle() const
{
	return  m_DescriptorHandles.at(m_RecentBoundType);
}

int Texture::GetHeapOffset() const
{
	return m_DescriptorHandles.at(m_RecentBoundType).heapOffset;
};

DXGI_FORMAT Texture::GetBaseFormat(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_TYPELESS;

	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8X8_TYPELESS;

		// 32-bit Z w/ Stencil
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_R32G8X24_TYPELESS;

		// No Stencil
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_TYPELESS;

		// 24-bit Z
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return DXGI_FORMAT_R24G8_TYPELESS;

		// 16-bit Z w/o Stencil
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_TYPELESS;

	default:
		return defaultFormat;
	}
};

DXGI_FORMAT Texture::GetDepthFormat(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
		// 32-bit Z w/ Stencil
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

		// No Stencil
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;

		// 24-bit Z
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		// 16-bit Z w/o Stencil
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;

	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}