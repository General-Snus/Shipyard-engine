#include "GraphicsEngine.pch.h"

#include <d3dcompiler.h> 
#include <DirectX/XTK/BufferHelpers.h>
#include <DirectX/XTK/CommonStates.h>
#include <DirectX/XTK/DDSTextureLoader.h> 
#include <DirectX/XTK/DirectXHelpers.h>
#include <DirectX/XTK/GraphicsMemory.h>
#include <DirectX/XTK/ResourceUploadBatch.h> 
#include <DirectXMath.h> 
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "../GPU.h" 
#include "../Helpers.h"
#include "../PSO.h"
#include "Editor/Editor/Windows/Window.h"  
#include "Tools/Logging/Logging.h"





bool GPU::Initialize(HWND aWindowHandle,bool enableDeviceDebug,const std::shared_ptr<Texture>& aBackBuffer,const std::shared_ptr<Texture>& aDepthBuffer)
{
	aWindowHandle; enableDeviceDebug; m_BackBuffer = aBackBuffer; m_DepthBuffer = aDepthBuffer;

	m_Swapchain = std::make_unique<GPUSwapchain>();
	m_CommandQueue = std::make_unique<GPUCommandQueue>();

	UINT dxgiFactoryFlags = 0;
	if (enableDeviceDebug)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}


	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags,IID_PPV_ARGS(&factory))))
	{
		Logger::Err("Failed to create DXGI factory");
	}

	ComPtr<IDXGIAdapter1> hardwareAdapter;
	if (m_useWarpDevice)
	{/*
		ComPtr<IDXGIAdapter> warpAdapter;*/
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter))))
		{
			Logger::Err("Failed to get warp adapter");
		}

		if (FAILED(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_Device)
		)))
		{
			Logger::Err("Failed to create warp adapter");
		}
	}
	else
	{
		GetHardwareAdapter(factory.Get(),&hardwareAdapter);

		if (FAILED(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_Device)
		)))
		{
			Logger::Err("Failed to create device");
		}
	}


	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS caps{};
	caps.pFeatureLevelsRequested = feature_levels;
	caps.NumFeatureLevels = ARRAYSIZE(feature_levels);
	if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,&caps,sizeof(D3D12_FEATURE_DATA_FEATURE_LEVELS))))
	{
		Logger::Err("Failed to Check FeatureSupport");
		return false;
	}
	if (FAILED(D3D12CreateDevice(hardwareAdapter.Get(),caps.MaxSupportedFeatureLevel,IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf()))))
	{
		Logger::Err("Failed to create device");
		return false;
	}

	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(m_Device.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION,TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR,TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING,TRUE);
	}

	m_DeviceSupport.targetFeatureLevel = caps.MaxSupportedFeatureLevel;
	m_CommandQueue->Create(m_Device,D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Describe and create the swap chain. 
	m_Swapchain->Create(aWindowHandle,m_CommandQueue->GetCommandQueue(),Window::Width(),Window::Height(),m_FrameCount);
	m_FrameIndex = m_Swapchain->m_SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps. 
	m_RtvHeap = CreateDescriptorHeap(m_Device,D3D12_DESCRIPTOR_HEAP_TYPE_RTV,m_FrameCount);
	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	m_SrvHeap = CreateDescriptorHeap(m_Device,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);



	// Create frame resources.
	UpdateRenderTargetViews(m_Device,m_Swapchain->m_SwapChain,m_RtvHeap);

	//for (int i = 0; i < m_FrameCount; i++)
	//{
	//	m_Allocator[i] = CreateCommandAllocator(m_Device,D3D12_COMMAND_LIST_TYPE_DIRECT);
	//}
	//
	//m_CommandList = CreateCommandList(m_Device,m_Allocator[m_FrameIndex],D3D12_COMMAND_LIST_TYPE_DIRECT);
	//g_Fence = CreateFence(m_Device);
	//g_FenceEvent = CreateEventHandle();


	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,&featureData,sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0,nullptr,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,&signature,&error)))
	{
		Logger::Err("Failed to Serialize RootSignature");
	}

	if (FAILED(m_Device->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&m_RootSignature.m_RootSignature))))
	{
		Logger::Err("Failed to Create RootSignature");
	}

	m_GraphicsMemory = std::make_unique<DirectX::DX12::GraphicsMemory>(m_Device.Get());

	m_BackBuffer->AllocateTexture(1920,1080);
	ResizeDepthBuffer(1920,1080);
	m_CommandQueue->ExecuteCommandList(m_CommandQueue->GetCommandList());

	return true;
}

bool GPU::UnInitialize()
{
	m_CommandQueue->Flush();
	//::CloseHandle(g_FenceEvent);

	return true;
}

void GPU::Present(unsigned aSyncInterval)
{
	aSyncInterval;
	auto list = m_CommandQueue->GetCommandList();
	TransitionResource(list.Get(),m_renderTargets[m_FrameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_PRESENT);

	m_FenceValues[m_FrameIndex] = m_CommandQueue->ExecuteCommandList(list);


	/*UINT syncInterval = m_VSync ? 1 : 0;
	UINT presentFlags = m_IsTearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;*/
	Helpers::ThrowIfFailed(m_Swapchain->m_SwapChain->Present(aSyncInterval,0));
	m_GraphicsMemory->Commit(m_CommandQueue->GetCommandQueue().Get());
	m_FrameIndex = m_Swapchain->m_SwapChain->GetCurrentBackBufferIndex();

	m_CommandQueue->WaitForFenceValue(m_FenceValues[m_FrameIndex]);






}

void GPU::UpdateBufferResource(
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource,
	size_t numElements,size_t elementSize,
	const void* bufferData,D3D12_RESOURCE_FLAGS flags
)
{

	const size_t bufferSize = numElements * elementSize;
	{
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,flags);
		Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(pDestinationResource)));
	}

	// Create an committed resource for the upload.
	if (bufferData)
	{
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource)));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = bufferData;
		subResourceData.RowPitch = bufferSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;


		UpdateSubresources(commandList.Get(),*pDestinationResource,*pIntermediateResource,0,0,1,&subResourceData);
	}
}

void GPU::ConfigureInputAssembler(
	const ComPtr<ID3D12GraphicsCommandList>& commandList,D3D_PRIMITIVE_TOPOLOGY topology,
	const D3D12_VERTEX_BUFFER_VIEW& vertView,const D3D12_INDEX_BUFFER_VIEW& indexView)
{
	commandList->IASetPrimitiveTopology(topology);
	commandList->IASetVertexBuffers(0,1,&vertView);
	commandList->IASetIndexBuffer(&indexView);
}

//bool GPU::CreateIndexBuffer(ComPtr<ID3D12Resource>& outIndexBuffer,const std::vector<unsigned>& aIndexList)
//{
//
//	const size_t size = sizeof(unsigned);
//	const size_t numIn = aIndexList.size();
//
//
//
//
//	ComPtr<ID3D12Resource> intermediateIndexBuffer;
//	UpdateBufferResource(
//		m_CommandQueue->GetCommandList(),
//		&outIndexBuffer,&intermediateIndexBuffer
//		,numIn,size,aIndexList.data()
//	);
//	return true;
//}

bool GPU::CreateIndexBuffer(D3D12_INDEX_BUFFER_VIEW& outIndexBufferView,ComPtr<ID3D12Resource>& outIndexBuffer,const std::vector<unsigned>& aIndexList)
{
	const D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(aIndexList));
	Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&outIndexBuffer)));

	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0,0);
	Helpers::ThrowIfFailed(
		outIndexBuffer->Map(0,&readRange,reinterpret_cast<void**>(&pVertexDataBegin))
	);
	memcpy(pVertexDataBegin,aIndexList.data(),sizeof(aIndexList));
	outIndexBuffer->Unmap(0,nullptr);

	// Initialize the vertex buffer view.
	outIndexBufferView.BufferLocation = outIndexBuffer->GetGPUVirtualAddress();
	outIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	outIndexBufferView.SizeInBytes = static_cast<unsigned>(aIndexList.size() * sizeof(unsigned));
	return false;
}

bool GPU::CreatePixelShader(ComPtr<ID3DBlob>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags)
{
	Helpers::ThrowIfFailed(D3DCompile(someShaderData,aShaderDataSize,nullptr,nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,nullptr,"ps_5_1",CompileFLags,0,&outPxShader,nullptr));
	return true;

}

bool GPU::CreateVertexShader(ComPtr<ID3DBlob>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags)
{
	Helpers::ThrowIfFailed(D3DCompile(someShaderData,aShaderDataSize,nullptr,nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,nullptr,"vs_5_1",CompileFLags,0,&outVxShader,nullptr));

	return true;
}

bool GPU::CreateDepthStencil(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	depthStencilDesc;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(m_Device->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(&m_RtvHeap))))
	{
		Logger::Err("Failed to make descriptor heap");
		return false;
	}
	return true;
}

void GPU::ResizeDepthBuffer(unsigned width,unsigned height)
{
	if (m_DepthBuffer->IsValid())
	{
		// Flush any GPU commands that might be referencing the depth buffer.
		m_CommandQueue->Flush();

		width = std::max(1u,width);
		height = std::max(1u,height);
		// Resize screen dependent resources.
		   // Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,width,height,
			1,0,1,0,D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_DepthBuffer->m_pResource)
		));
		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		m_Device->CreateDepthStencilView(m_DepthBuffer->GetResource(),&dsv,
			m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

bool GPU::LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName,bool generateMips)
{
	outTexture; aFileName;

	if (!std::filesystem::exists(aFileName) && aFileName.extension() != "dds")
	{
		std::string error = "Failed to load texture: " + aFileName.string() + " does not exist!";
		std::cout << error << std::endl;
		return false;
	}

	ResourceUploadBatch resourceUpload(m_Device.Get());
	resourceUpload.Begin();


	Helpers::ThrowIfFailed(
		CreateDDSTextureFromFile(m_Device.Get(),resourceUpload,L"texture.dds",
			outTexture->m_pResource.ReleaseAndGetAddressOf(),generateMips)
	);

	CreateShaderResourceView(m_Device.Get(),outTexture->m_pResource.Get(),
		outTexture->m_hCpuDescriptorHandle);

	// Upload the resources to the GPU.
	auto uploadResourcesFinished = resourceUpload.End(m_CommandQueue->GetCommandQueue().Get());

	// Wait for the upload thread to terminate
	uploadResourcesFinished.wait();

	//else if (aFileName.extension() == ".hdr")
	//{
	//	Helpers::ThrowIfFailed(LoadFromHDRFile(fileName.c_str(),&metadata,scratchImage));
	//}
	//else if (aFileName.extension() == ".tga")
	//{
	//	Helpers::ThrowIfFailed(LoadFromTGAFile(fileName.c_str(),&metadata,scratchImage));
	//} 

	//assert(outTexture && "Please initialize the Texture Object before calling this function!");
	////outTexture = std::make_shared<Texture>();
	//outTexture->myName = aFileName;/*
	//outTexture->myBindFlags = D3D12_BIND_SHADER_RESOURCE;
	//outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	//outTexture->myAccessFlags = 0;*/
	//HRESULT result = DirectX::CreateDDSTextureFromFile(
	//	m_Device,
	//	aFileName.c_str(),
	//	outTexture->myTexture.GetAddressOf(),
	//	outTexture->mySRV.GetAddressOf()
	//);

	//if (FAILED(result))
	//{
	//	Logger::Err("Failed to load the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
	//	return false;
	//}

	//result = m_Device->CreateShaderResourceView(outTexture->myTexture.Get(),nullptr,outTexture->mySRV.GetAddressOf());
	//if (FAILED(result))
	//{
	//	Logger::Err("Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
	//	return false;
	//}

	//std::wstring textureName = aFileName;
	//if (const size_t pos = textureName.find_last_of(L'\\'); pos != std::wstring::npos)
	//{
	//	textureName = textureName.substr(pos + 1);
	//}

	//textureName = textureName.substr(0,textureName.size() - 4);
	//outTexture->myName = textureName;

	return true;
}

bool GPU::LoadTextureFromMemory(Texture* outTexture,const std::filesystem::path& aName,const BYTE* someImageData,size_t anImageDataSize,const D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	outTexture; aName; someImageData; anImageDataSize; aSRVDesc;
	return false;
}

void GPU::TransitionResource(
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12Resource>& resource,D3D12_RESOURCE_STATES beforeState,
	D3D12_RESOURCE_STATES afterState)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		beforeState,afterState);

	commandList->ResourceBarrier(1,&barrier);
}

void GPU::ClearRTV(const ComPtr<ID3D12GraphicsCommandList>& commandList,D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT* clearColor)
{
	commandList->ClearRenderTargetView(rtv,clearColor,0,nullptr);
}

void GPU::ClearDepth(const ComPtr<ID3D12GraphicsCommandList>& commandList,D3D12_CPU_DESCRIPTOR_HANDLE dsv,FLOAT depth)
{
	commandList->ClearDepthStencilView(dsv,D3D12_CLEAR_FLAG_DEPTH,depth,0,0,nullptr);
}

ComPtr<ID3D12Resource> GPU::GetCurrentRenderTargetView()
{
	return m_renderTargets[m_FrameIndex];//todo fix
}

ComPtr<ID3D12Resource> GPU::GetCurrentBackBuffer()
{
	return m_BackBuffer->GetResource();//todo fix
}

ComPtr<ID3D12DescriptorHeap>  GPU::CreateDescriptorHeap(const ComPtr<ID3D12Device>& device,D3D12_DESCRIPTOR_HEAP_TYPE type,uint32_t numDescriptors,D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	Helpers::ThrowIfFailed(device->CreateDescriptorHeap(&desc,IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}

ComPtr<ID3D12Fence> GPU::CreateFence(const ComPtr<ID3D12Device>& device)
{
	ComPtr<ID3D12Fence> fence;

	Helpers::ThrowIfFailed(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence)));

	return fence;
}

HANDLE GPU::CreateEventHandle()
{
	HANDLE fenceEvent;

	fenceEvent = ::CreateEvent(nullptr,FALSE,FALSE,nullptr);
	assert(fenceEvent && "Failed to create fence event.");

	return fenceEvent;
}

void GPU::UpdateRenderTargetViews(const ComPtr<ID3D12Device>& device,const ComPtr<IDXGISwapChain4>& swapChain,const ComPtr<ID3D12DescriptorHeap>& descriptorHeap)
{
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < m_FrameCount; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		Helpers::ThrowIfFailed(swapChain->GetBuffer(i,IID_PPV_ARGS(&backBuffer)));
		device->CreateRenderTargetView(backBuffer.Get(),nullptr,rtvHandle);
		m_renderTargets[i] = backBuffer;
		rtvHandle.Offset(rtvDescriptorSize);
	}
}

//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
void GPU::GetHardwareAdapter(IDXGIFactory1* pFactory,IDXGIAdapter1** ppAdapter,bool requestHighPerformanceAdapter)
{
	pFactory; ppAdapter; requestHighPerformanceAdapter;
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (
			UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter)));
				++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,_uuidof(ID3D12Device),nullptr)))
			{
				break;
			}
		}
	}

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex,&adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,_uuidof(ID3D12Device),nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

void GPUSwapchain::Create(HWND hwnd,ComPtr<ID3D12CommandQueue>,UINT Width,UINT Height,UINT bufferCount)
{
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	Helpers::ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags,IID_PPV_ARGS(&dxgiFactory4)));


	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = Width;
	swapChainDesc.Height = Height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	ComPtr<IDXGISwapChain1> swapChain;
	if (FAILED(dxgiFactory4->CreateSwapChainForHwnd(
		GPU::m_CommandQueue->GetCommandQueue().Get(),        // Swap chain needs the queue so that it can force a flush on it.
		Window::windowHandler,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	)))
	{
		Logger::Err("Failed to create swapchain from hwnd");
	}
	Helpers::ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hwnd,DXGI_MWA_NO_ALT_ENTER));
	Helpers::ThrowIfFailed(swapChain.As(&m_SwapChain));
}
