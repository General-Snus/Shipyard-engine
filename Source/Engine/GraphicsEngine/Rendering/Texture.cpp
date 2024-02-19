#include "GraphicsEngine.pch.h" 


bool Texture::AllocateTexture(const unsigned width,const unsigned height)
{
	ComPtr<ID3D12Resource> textureUploadHeap;
	// Create the texture.
	{
		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;


		auto descProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
			&descProperty,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pResource)));

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(),0,1);
		auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		// Create the GPU upload buffer.
		Helpers::ThrowIfFailed(GPU::m_Device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		std::vector<UINT8> texture;
		texture.resize(width * height * 4);

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &texture[0];
		textureData.RowPitch = width * 4;
		textureData.SlicePitch = textureData.RowPitch * height;
		UpdateSubresources(GPU::m_CommandQueue->GetCommandList().Get(),m_pResource.Get(),textureUploadHeap.Get(),0,0,1,&textureData);


		GPU::TransitionResource(GPU::m_CommandQueue->GetCommandList(),textureUploadHeap,D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		GPU::m_Device->CreateShaderResourceView(m_pResource.Get(),&srvDesc,GPU::m_SrvHeap->GetCPUDescriptorHandleForHeapStart());
	}
	return false;
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