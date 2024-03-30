#include "DirectXHeader.pch.h"

#include <XTK/DDSTextureLoader.h>
#include <XTK/ResourceUploadBatch.h>
#include <XTK/WICTextureLoader.h>
#include "../GPU.h" 

#include "../Helpers.h" 
#include "Engine/GraphicsEngine/Shaders/Registers.h"
#include "Shipyard/CommandQueue.h"
#include "Shipyard/eDescriptors.h"
#include "Shipyard/ResourceStateTracker.h"
#include "Shipyard/Texture.h"


bool GPU::Initialize(HWND aWindowHandle,bool enableDeviceDebug,const std::shared_ptr<Texture>& aBackBuffer,const std::shared_ptr<Texture>& aDepthBuffer
	,uint32_t width,uint32_t height)
{
	aWindowHandle; enableDeviceDebug; m_BackBuffer = aBackBuffer; m_DepthBuffer = aDepthBuffer;

	m_Height = height;
	m_Width = width;

	m_Swapchain = std::make_unique<GPUSwapchain>();

	m_DirectCommandQueue = std::make_shared<GPUCommandQueue>();
	m_CopyCommandQueue = std::make_shared<GPUCommandQueue>();
	m_ComputeCommandQueue = std::make_shared<GPUCommandQueue>();

	UINT dxgiFactoryFlags = 0;
	if (enableDeviceDebug)
	{
#if !USE_NSIGHT_AFTERMATH
		ComPtr<ID3D12Debug3> debugController;
		const auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		if (SUCCEEDED(result))
		{
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(TRUE);
			debugController->SetEnableSynchronizedCommandQueueValidation(TRUE);
			//debugController->SetEnableAutoName(TRUE);
			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings))))
		{
			// Turn on AutoBreadcrumbs and Page Fault reporting
			pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}
#endif
	}


#if USE_NSIGHT_AFTERMATH 
	m_hAftermathCommandListContext = nullptr;
	m_gpuCrashTracker = new GpuCrashTracker(m_markerMap);
	m_frameCounter = 0;
	// Enable Nsight Aftermath GPU crash dump creation.
	// This needs to be done before the D3D device is created.
	m_gpuCrashTracker->Initialize();
#endif 


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

#if (USE_NSIGHT_AFTERMATH)
	const uint32_t aftermathFlags =
		GFSDK_Aftermath_FeatureFlags_EnableMarkers |             // Enable event marker tracking.
		GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |    // Enable tracking of resources.
		GFSDK_Aftermath_FeatureFlags_CallStackCapturing |        // Capture call stacks for all draw calls, compute dispatches, and resource copies.
		GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;    // Generate debug information for shaders.

	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_DX12_Initialize(
		GFSDK_Aftermath_Version_API,
		aftermathFlags,
		m_Device.Get()));
#endif

	CD3DX12FeatureSupport featureSupport;
	featureSupport.Init(m_Device.Get());
	m_FeatureData = featureSupport.HighestRootSignatureVersion();
	m_DeviceSupport.targetFeatureLevel = featureSupport.MaxSupportedFeatureLevel();
	m_DeviceSupport.targetShaderModel = featureSupport.HighestShaderModel();

	if (FAILED(D3D12CreateDevice(hardwareAdapter.Get(),m_DeviceSupport.targetFeatureLevel,IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf()))))
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

	m_GraphicsMemory = std::make_shared<DirectX::DX12::GraphicsMemory>(m_Device.Get());


	m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV] = std::make_unique<DescriptorPile>(
		m_Device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		2048
	);

	m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_SAMPLER] = std::make_unique<DescriptorPile>(
		m_Device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		2048
	);

	m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_RTV] = std::make_unique<DescriptorPile>(
		m_Device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		2048
	);
	m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_DSV] = std::make_unique<DescriptorPile>(
		m_Device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		2048
	);



	ComPtr<ID3D12DebugDevice1> pDebugQueue;
	if (SUCCEEDED(m_Device.As(&pDebugQueue)))
	{
	}

	m_DirectCommandQueue->Create(m_Device,D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_CopyCommandQueue->Create(m_Device,D3D12_COMMAND_LIST_TYPE_COPY);
	m_ComputeCommandQueue->Create(m_Device,D3D12_COMMAND_LIST_TYPE_COMPUTE);

	// Describe and create the swap chain. 
	m_Swapchain->Create(aWindowHandle,m_DirectCommandQueue->GetCommandQueue(),width,height,m_FrameCount);
	m_FrameIndex = m_Swapchain->m_SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps. 
	//m_RtvHeap = CreateDescriptorHeap(m_Device,D3D12_DESCRIPTOR_HEAP_TYPE_RTV,m_FrameCount);
	//m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//m_DsvHeap = CreateDescriptorHeap(m_Device,D3D12_DESCRIPTOR_HEAP_TYPE_DSV,1);
	//m_SrvHeap = CreateDescriptorHeap(m_Device,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);



	// Create frame resources.
	UpdateRenderTargetViews(m_Device,m_Swapchain->m_SwapChain,nullptr);

	m_BackBuffer->AllocateTexture({ width,height },"Backbuffer");
	ResizeDepthBuffer(width,height);


	//m_DirectCommandQueue->ExecuteCommandList(m_DirectCommandQueue->GetCommandList());

	m_Viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
	m_ScissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };


	return true;
}

bool GPU::UnInitialize()
{
	m_DirectCommandQueue->Flush();
	m_CopyCommandQueue->Flush();
	m_ComputeCommandQueue->Flush();
	m_GraphicsMemory.reset();

	//::CloseHandle(g_FenceEvent);

	return true;
}

void GPU::Present(unsigned aSyncInterval)
{
	const auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	const auto commandList = commandQueue->GetCommandList();

	commandList->TransitionBarrier(m_renderTargets[m_FrameIndex].GetResource(),D3D12_RESOURCE_STATE_PRESENT);
	commandQueue->ExecuteCommandList(commandList);

#if  (USE_NSIGHT_AFTERMATH)
	auto hr = (m_Swapchain->m_SwapChain->Present(aSyncInterval,0));
	if (FAILED(hr))
	{
		// DXGI_ERROR error notification is asynchronous to the NVIDIA display
		// driver's GPU crash handling. Give the Nsight Aftermath GPU crash dump
		// thread some time to do its work before terminating the process.
		auto tdrTerminationTimeout = std::chrono::seconds(3);
		auto tStart = std::chrono::steady_clock::now();
		auto tElapsed = std::chrono::milliseconds::zero();

		GFSDK_Aftermath_CrashDump_Status status = GFSDK_Aftermath_CrashDump_Status_Unknown;
		AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GetCrashDumpStatus(&status));

		while (status != GFSDK_Aftermath_CrashDump_Status_CollectingDataFailed &&
			status != GFSDK_Aftermath_CrashDump_Status_Finished &&
			tElapsed < tdrTerminationTimeout)
		{
			// Sleep 50ms and poll the status again until timeout or Aftermath finished processing the crash dump.
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GetCrashDumpStatus(&status));

			auto tEnd = std::chrono::steady_clock::now();
			tElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart);
		}

		if (status != GFSDK_Aftermath_CrashDump_Status_Finished)
		{
			std::stringstream err_msg;
			err_msg << "Unexpected crash dump status: " << status;
			MessageBoxA(NULL,err_msg.str().c_str(),"Aftermath Error",MB_OK);
		}

		// Terminate on failure
		GPU::UnInitialize();
		exit(-1);
		return;
	}
#else 
	Helpers::ThrowIfFailed(m_Swapchain->m_SwapChain->Present(aSyncInterval,0));
#endif
	m_FenceValues[m_FrameIndex] = commandQueue->Signal();
	m_FrameIndex = m_Swapchain->m_SwapChain->GetCurrentBackBufferIndex();

	//GraphicsMemory::Get().Commit(commandQueue->GetCommandQueue().Get());
	commandQueue->WaitForFenceValue(m_FenceValues[m_FrameIndex]);
}

void GPU::UpdateBufferResource(
	const CommandList& commandList,
	ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource,
	size_t numElements,size_t elementSize,
	const void* bufferData,D3D12_RESOURCE_FLAGS flags
)
{

	const size_t bufferSize = numElements * elementSize;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,flags);

	Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource)));

	// Create an committed resource for the upload.
	if (bufferData)
	{
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

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


		UpdateSubresources(commandList.GetGraphicsCommandList().Get(),*pDestinationResource,*pIntermediateResource,0,0,1,&subResourceData);
	}
}

void GPU::ConfigureInputAssembler(
	CommandList& commandList,D3D_PRIMITIVE_TOPOLOGY topology,
	VertexResource& vertexResource,IndexResource& indexResource)
{
	commandList.GetGraphicsCommandList()->IASetPrimitiveTopology(topology);

	commandList.TransitionBarrier(vertexResource,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	const auto& vertView = vertexResource.GetVertexBufferView();
	commandList.GetGraphicsCommandList()->IASetVertexBuffers(0,1,&vertView);
	commandList.TrackResource(vertexResource);

	commandList.TransitionBarrier(indexResource,D3D12_RESOURCE_STATE_INDEX_BUFFER);
	const auto& indexView = indexResource.GetIndexBufferView();
	commandList.GetGraphicsCommandList()->IASetIndexBuffer(&indexView);
	commandList.TrackResource(indexResource);
}
bool GPU::CreateIndexBuffer(const std::shared_ptr<CommandList>& commandList,IndexResource& outIndexResource,const std::vector<uint32_t>& aIndexList)
{
	const DXGI_FORMAT indexFormat = (sizeof(uint32_t) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	const size_t indexSizeInBytes = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
	commandList->CopyBuffer(outIndexResource,aIndexList.size(),indexSizeInBytes,aIndexList.data());
	return true;
}


bool GPU::CreateDepthStencil(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	depthStencilDesc;
	return true;
}

void GPU::ResizeDepthBuffer(unsigned width,unsigned height)
{
	m_DirectCommandQueue->Flush();

	width = std::max(1u,width);
	height = std::max(1u,height);

	const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		width,
		height,
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	const CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D32_FLOAT,0.0f,0u);
	Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthBuffer->m_Resource)
	));
	m_DepthBuffer->m_Resource->SetName(L"DepthBuffer");


	m_DepthBuffer->CheckFeatureSupport();
	m_DepthBuffer->SetView(ViewType::DSV);
}

bool GPU::LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName,bool generateMips)
{
	outTexture; aFileName;

	if (!std::filesystem::exists(aFileName) || aFileName.extension() != ".dds" && aFileName.extension() != ".png")
	{
		const std::string error = "Failed to load texture: " + aFileName.string() + " does not exist!";
		std::cout << error << std::endl;
		return false;
	}
	outTexture->myName = aFileName.filename().string();
	ResourceUploadBatch resourceUpload(m_Device.Get());
	resourceUpload.Begin();
	bool isCubeMap = false;
	if (aFileName.extension() == ".dds")
	{
		Helpers::ThrowIfFailed(
			CreateDDSTextureFromFile(m_Device.Get(),resourceUpload,aFileName.wstring().c_str(),
				outTexture->m_Resource.ReleaseAndGetAddressOf(),generateMips,0,nullptr,&isCubeMap)
		);
	}
	else if (aFileName.extension() == ".png")
	{
		Helpers::ThrowIfFailed(
			CreateWICTextureFromFile(m_Device.Get(),resourceUpload,aFileName.wstring().c_str(),
				outTexture->m_Resource.ReleaseAndGetAddressOf(),generateMips)
		);

	}

	outTexture->isCubeMap = isCubeMap;
	const auto uploadResourcesFinished = resourceUpload.End(m_DirectCommandQueue->GetCommandQueue().Get());
	uploadResourcesFinished.wait();

	outTexture->m_Width = static_cast<uint32_t>(outTexture->m_Resource->GetDesc().Width);
	outTexture->m_Height = outTexture->m_Resource->GetDesc().Height;
	outTexture->m_Resource->SetName(aFileName.wstring().c_str());

	outTexture->CheckFeatureSupport();
	outTexture->SetView(ViewType::SRV);
	return true;
}

bool GPU::LoadTextureFromMemory(Texture* outTexture,const std::filesystem::path& aName,const BYTE* someImageData,size_t anImageDataSize,bool generateMips,const D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	outTexture; aName; someImageData; anImageDataSize; aSRVDesc;
	outTexture->myName = aName.string();
	//outTexture->m_DescriptorHandle = std::make_unique<DescriptorHeap>(m_Device.Get(),
	//	eDescriptors::Textures);


	ResourceUploadBatch resourceUpload(m_Device.Get());
	resourceUpload.Begin();

	Helpers::ThrowIfFailed(
		CreateDDSTextureFromMemory(m_Device.Get(),resourceUpload,someImageData,anImageDataSize,
			outTexture->m_Resource.ReleaseAndGetAddressOf(),generateMips)
	);

	const auto uploadResourcesFinished = resourceUpload.End(m_DirectCommandQueue->GetCommandQueue().Get());
	uploadResourcesFinished.wait();

	outTexture->SetView(ViewType::SRV);
	return true;
}

void GPU::TransitionResource(
	const CommandList& commandList,
	const ComPtr<ID3D12Resource>& resource,D3D12_RESOURCE_STATES beforeState,
	D3D12_RESOURCE_STATES afterState)
{
	const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		beforeState,afterState);

	commandList.GetGraphicsCommandList()->ResourceBarrier(1,&barrier);
}

void GPU::ClearRTV(const CommandList& commandList,D3D12_CPU_DESCRIPTOR_HANDLE rtv,Vector4f clearColor)
{
	commandList.GetGraphicsCommandList()->ClearRenderTargetView(rtv,&clearColor.x,0,nullptr);
}

void GPU::ClearRTV(const CommandList& commandList,Texture* rtv,unsigned textureCount)
{
	for (unsigned i = 0; i < textureCount; ++i)
	{
		commandList.GetGraphicsCommandList()->ClearRenderTargetView(rtv[i].GetHandle(ViewType::RTV).cpuPtr,&rtv->m_ClearColor.x,0,nullptr);
	}
}

void GPU::ClearDepth(const CommandList& commandList,D3D12_CPU_DESCRIPTOR_HANDLE dsv,FLOAT depth)
{
	commandList.GetGraphicsCommandList()->ClearDepthStencilView(dsv,D3D12_CLEAR_FLAG_DEPTH,depth,0,0,nullptr);
}


D3D12_CPU_DESCRIPTOR_HANDLE GPU::GetCurrentRenderTargetView()
{
	return m_renderTargets[m_FrameIndex].GetHandle(ViewType::RTV).cpuPtr;
}

Texture* GPU::GetCurrentBackBuffer()
{
	return &m_renderTargets[m_FrameIndex];//todo fix fixed
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

std::shared_ptr<GPUCommandQueue> GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
	std::shared_ptr<GPUCommandQueue> commandQueue;

	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		commandQueue = m_DirectCommandQueue;
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		commandQueue = m_ComputeCommandQueue;
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		commandQueue = m_CopyCommandQueue;
		break;
	default:
		assert(false && "Invalid command queue type.");
	}

	return commandQueue;
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
	descriptorHeap; device;
	for (int i = 0; i < m_FrameCount; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		Helpers::ThrowIfFailed(swapChain->GetBuffer(i,IID_PPV_ARGS(&backBuffer)));

		ResourceStateTracker::AddGlobalResourceState(backBuffer.Get(),D3D12_RESOURCE_STATE_COMMON);

		m_renderTargets[i].SetResource(backBuffer);
		m_renderTargets[i].SetView(ViewType::RTV);
		m_renderTargets->myName = "backbuffer: " + std::to_string(i);
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
		GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue().Get(),        // Swap chain needs the queue so that it can force a flush on it.
		hwnd,
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


#if  (USE_NSIGHT_AFTERMATH)
// A helper for setting Aftermath event markers.
// For maximum CPU performance, use GFSDK_Aftermath_SetEventMarker() with dataSize=0.
// This instructs Aftermath not to allocate and copy off memory internally, relying on
// the application to manage marker pointers itself.
void GPU::setAftermathEventMarker(const std::string& markerData,bool appManagedMarker)
{
	if (appManagedMarker)
	{
		// App is responsible for handling marker memory, and for resolving the memory at crash dump generation time.
		// The actual "const void* markerData" passed to Aftermath in this case can be any uniquely identifying value that the app can resolve to the marker data later.
		// For this sample, we will use this approach to generating a unique marker value:
		// We keep a ringbuffer with a marker history of the last c_markerFrameHistory frames (currently 4).
		UINT markerMapIndex = m_frameCounter % GpuCrashTracker::c_markerFrameHistory;
		auto& currentFrameMarkerMap = m_markerMap[markerMapIndex];
		// Take the index into the ringbuffer, multiply by 10000, and add the total number of markers logged so far in the current frame, +1 to avoid a value of zero.
		size_t markerID = markerMapIndex * 10000 + currentFrameMarkerMap.size() + 1;
		// This value is the unique identifier we will pass to Aftermath and internally associate with the marker data in the map.
		currentFrameMarkerMap[markerID] = markerData;
		AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_SetEventMarker(m_hAftermathCommandListContext,(void*)markerID,0));
		// For example, if we are on frame 625, markerMapIndex = 625 % 4 = 1...
		// The first marker for the frame will have markerID = 1 * 10000 + 0 + 1 = 10001.
		// The 15th marker for the frame will have markerID = 1 * 10000 + 14 + 1 = 10015.
		// On the next frame, 626, markerMapIndex = 626 % 4 = 2.
		// The first marker for this frame will have markerID = 2 * 10000 + 0 + 1 = 20001.
		// The 15th marker for the frame will have markerID = 2 * 10000 + 14 + 1 = 20015.
		// So with this scheme, we can safely have up to 10000 markers per frame, and can guarantee a unique markerID for each one.
		// There are many ways to generate and track markers and unique marker identifiers!
	}
	else
	{
		AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_SetEventMarker(m_hAftermathCommandListContext,(void*)markerData.c_str(),(unsigned int)markerData.size() + 1));
	}
};

// A helper that prepends the frame number to a string
std::string GPU::createMarkerStringForFrame(const char* markerString) {
	std::stringstream ss;
	ss << "Frame " << m_frameCounter << ": " << markerString;
	return ss.str();
};
#endif