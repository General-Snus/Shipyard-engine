#include "DirectXHeader.pch.h"

#include <memory>

#include "Graphics/Texture.h"
#include "Graphics/CommandQueue.h"
#include "Graphics/eDescriptors.h"
#include "Graphics/GPU.h"


using namespace DirectX;

Texture::Texture() : m_Viewport(0,0,0,0,0,1),m_Rect(0,0,0,0) {
	auto value = HeapHandle(D3D12_CPU_DESCRIPTOR_HANDLE(),D3D12_GPU_DESCRIPTOR_HANDLE(),-1);
	m_DescriptorHandles.emplace(ViewType::SRV,value);
};

struct free_deleter {
	void operator()(void* const ptr) const {
		free(ptr);
	}
};

template <typename T>
using free_unique_ptr = std::unique_ptr<T,free_deleter>;

bool Texture::AllocateTexture(const Vector2ui dimentions,const std::filesystem::path& name,const Vector4f& ClearColor,
	DXGI_FORMAT Format,D3D12_RESOURCE_FLAGS flags,D3D12_RESOURCE_STATES targetResourceState) {
	OPTICK_EVENT();
	const auto width = dimentions.x;
	const auto height = dimentions.y;
	m_ClearColor = ClearColor;

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

	const CD3DX12_CLEAR_VALUE clearValue = {Format, &m_ClearColor.x};

	Helpers::ThrowIfFailed(GPUInstance.m_Device->CreateCommittedResource(
		&heapProps,D3D12_HEAP_FLAG_NONE,&txtDesc,D3D12_RESOURCE_STATE_COMMON,&clearValue,
		IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())));
	m_Resource->SetName(name.wstring().c_str());

	for(int i = 0; i < 4; i++) {
		m_ClearColor[i] = clearValue.Color[i];
	}

	const auto                pixelSize = BitsPerPixel(Format);
	static constexpr uint32_t s_whitePixels = 0xFFFFFFFF;

	const size_t size = txtDesc.Height * txtDesc.Width * pixelSize;
	//auto       pixelData = std::make_unique<void*[]>(size);


	std::unique_ptr<void,free_deleter> pixelData(malloc(size));
	memset(pixelData.get(),s_whitePixels,size);
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pixelData.get();
	textureData.RowPitch = txtDesc.Width * pixelSize;
	textureData.SlicePitch = size;

	ResourceUploadBatch resourceUpload(GPUInstance.m_Device.Get());
	resourceUpload.Begin();
	resourceUpload.Upload(m_Resource.Get(),0,&textureData,1);
	resourceUpload.Transition(m_Resource.Get(),D3D12_RESOURCE_STATE_COPY_DEST,targetResourceState);

	const auto uploadResourcesFinished =
		resourceUpload.End(GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue().Get());

	uploadResourcesFinished.wait();
	if(!uploadResourcesFinished.valid()) {
		throw std::runtime_error("Failed to upload texture");
	}

	CheckFeatureSupport();
	if(targetResourceState == D3D12_RESOURCE_STATE_RENDER_TARGET) {
		SetView(ViewType::RTV);
	}

	return false;
}

void Texture::SetView(D3D12_SHADER_RESOURCE_VIEW_DESC view) {
	OPTICK_EVENT();
	if(!m_Resource ||
		m_DescriptorHandles.contains(ViewType::SRV) && m_DescriptorHandles.at(ViewType::SRV).heapOffset != -1) {
		return;
	}

	const HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
	GPUInstance.m_Device->CreateShaderResourceView(m_Resource.Get(),&view,handle.cpuPtr);
	m_DescriptorHandles[ViewType::SRV] = handle;
}

void Texture::SetView(D3D12_UNORDERED_ACCESS_VIEW_DESC view) {
	view;
}

void Texture::SetView(ViewType view) {
	OPTICK_EVENT();
	if(!m_Resource || m_DescriptorHandles.contains(view) && m_DescriptorHandles.at(view).heapOffset != -1) {
		return;
	}

	auto device = GPUInstance.m_Device;
	CheckFeatureSupport();
	CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

	switch(view) {
	case ViewType::UAV:
	{
		HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
		CreateUnorderedAccessView(GPUInstance.m_Device.Get(),m_Resource.Get(),handle.cpuPtr,desc.MipLevels);
		m_DescriptorHandles[ViewType::UAV] = handle;
	}
	break;
	case ViewType::SRV:
	{
		if(CheckDSVSupport()) {
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = Helpers::GetDepthFormat(m_Format);
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRVDesc.Texture2D.MipLevels = 1;

			HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
			GPUInstance.m_Device->CreateShaderResourceView(m_Resource.Get(),&SRVDesc,handle.cpuPtr);
			m_DescriptorHandles[ViewType::SRV] = handle;
		} else {
			HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV);
			CreateShaderResourceView(GPUInstance.m_Device.Get(),m_Resource.Get(),handle.cpuPtr,isCubeMap);
			m_DescriptorHandles[ViewType::SRV] = handle;
		}
		break;
	}
	case ViewType::RTV:
	{
		HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_RTV);
		device->CreateRenderTargetView(m_Resource.Get(),nullptr,handle.cpuPtr);
		m_DescriptorHandles[ViewType::RTV] = handle;
		break;
	}
	case ViewType::DSV:
	{
		HeapHandle handle = GPUInstance.GetHeapHandle(eHeapTypes::HEAP_TYPE_DSV);

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

size_t Texture::GetMemorySize() {
	return BitsPerPixel(m_Resource->GetDesc().Format) * 8 * GetWidth() * GetHeight();
}

bool Texture::CopyDataInto(void* destination) {

	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	const auto commandList = commandQueue->GetCommandList();



	D3D12_RESOURCE_DESC resourceDesc = m_Resource->GetDesc();
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // Make sure no flags that prevent CPU access are set
	resourceDesc.Alignment = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_READBACK;

	ComPtr<ID3D12Resource> stagingResource;
	auto hr = GPUInstance.m_Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // Initial state
		nullptr, // Clear value
		IID_PPV_ARGS(&stagingResource)
	);

	if(FAILED(hr)) {
		return false;
	}

	commandList->TransitionBarrier(*this,D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->CopyResource(stagingResource.Get(),m_Resource);
	commandList->TransitionBarrier(*this,D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_COPY_SOURCE);

	auto fence = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fence);

	void* data;
	hr = stagingResource->Map(0,nullptr,&data);
	if(SUCCEEDED(hr)) {
		memcpy(destination,data,GetMemorySize());
		stagingResource->Unmap(0,nullptr);
		return true;
	}

	return false;
}

bool Texture::CopyDataInto(void* destination,Vector2ui pixel,Vector2ui rect) {
	destination; pixel; rect;
	return false;
}
