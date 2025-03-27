#include "DirectXHeader.pch.h"
#include "../Texture.h"

#include "Graphics/CommandQueue.h"
#include "Graphics/eDescriptors.h"
#include "Graphics/GPU.h" 
#include <DirectX/XTK/Src/LoaderHelpers.h>

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
	return CopyDataInto(destination,Vector2ui(0,0),Vector2ui(GetWidth(),GetHeight()));
}

//Rect will expand from top left corner to bottom right
bool Texture::CopyDataInto(void* destination,Vector2ui pixel,Vector2ui rect) {
 	OPTICK_EVENT();
	const auto desc = m_Resource->GetDesc();
	size_t rowPitch;
	size_t slicePitch;
	size_t rowCount;
	auto hr = DirectX::LoaderHelpers::GetSurfaceInfo(static_cast<size_t>(rect.x),rect.y,desc.Format,&slicePitch,&rowPitch,&rowCount);
	if(FAILED(hr))
		return hr;

	if(rowPitch > UINT32_MAX || slicePitch > UINT32_MAX)
		return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

	const uint64_t dstRowPitch = (rowPitch + 255) & ~0xFFu;

	if(dstRowPitch > UINT32_MAX)
		return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

	assert((dstRowPitch & 0xFF) == 0);
	assert(desc.SampleDesc.Count == 1); // https://github.com/microsoft/DirectXTK12/blob/67f987722b9eef4c2af96a4bb9abe1cc602d869c/Src/ScreenGrab.cpp#L127

	auto bufferSize = dstRowPitch * rect.y; 
	if(bufferSize > UINT32_MAX)
		return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

	//This resource was not initialized correctly
	if(m_TransitioningState == -1) {
		return SUCCEEDED(E_FAIL);
	}


	// The readback buffer (created below) is on a readback heap, so that the CPU can access it.
	const CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_HEAP_PROPERTIES readBackHeapProperties(D3D12_HEAP_TYPE_READBACK);

	D3D12_RESOURCE_DESC readbackBufferDesc{CD3DX12_RESOURCE_DESC::Buffer(bufferSize)};

	Ref<ID3D12Resource> pStaging;
	hr = GPUInstance.m_Device->CreateCommittedResource(
		&readBackHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&readbackBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pStaging.GetAddressOf()));

	if(FAILED(hr)) {
		//No need to release because ref counting com ptr?
		return false;
	}

	SetDebugObjectName(pStaging.Get(),L"ScreenGrab staging");
	assert(pStaging.Get());

	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	const auto commandList = commandQueue->GetCommandList();

	auto previousState = m_TransitioningState;
	commandList->TransitionBarrier(*this,D3D12_RESOURCE_STATE_COPY_SOURCE);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferFootprint = {};
	bufferFootprint.Footprint.Width = static_cast<UINT>(rect.x);
	bufferFootprint.Footprint.Height = rect.y;
	bufferFootprint.Footprint.Depth = 1;
	bufferFootprint.Footprint.RowPitch = static_cast<UINT>(dstRowPitch);
	bufferFootprint.Footprint.Format = desc.Format;

	const CD3DX12_TEXTURE_COPY_LOCATION copyDest(pStaging.Get(),bufferFootprint);
	const CD3DX12_TEXTURE_COPY_LOCATION copySrc(m_Resource.Get(),0);
	const CD3DX12_BOX sourceBox(pixel.x,pixel.y,pixel.x + rect.x,pixel.y + rect.y);

	// Copy the texture
	commandList->GetGraphicsCommandList()->CopyTextureRegion(&copyDest,0,0,0,&copySrc,&sourceBox);

	// Transition the source resource to the next state
	commandList->TransitionBarrier(*this,previousState);

	auto fence = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fence); 

	void* pMappedMemory = nullptr;
	D3D12_RANGE readRange = {0, static_cast<SIZE_T>(bufferSize)};
	D3D12_RANGE writeRange = {0, 0};
	hr = pStaging->Map(0,&readRange,&pMappedMemory);
	if(FAILED(hr))
		return hr;

	auto sptr = static_cast<const uint8_t*>(pMappedMemory);
	if(!sptr) {
		pStaging->Unmap(0,&writeRange);
		return SUCCEEDED(E_POINTER);
	}


	auto* dptr = (uint8_t*)destination;

	const size_t msize = std::min<size_t>(rowPitch,size_t(dstRowPitch));
	for(size_t h = 0; h < rowCount; ++h) {
		memcpy(dptr,sptr,msize);
		sptr += dstRowPitch;
		dptr += rowPitch;
	}

	pStaging->Unmap(0,&writeRange);

	return true;

}
