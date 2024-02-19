#pragma once
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include <cstdint>
#include <DirectX/XTK/GraphicsMemory.h>
#include <DirectX\directx\d3d12.h> 
#include <DirectX\directx\d3dcommon.h>
#include <dxgi.h>
#include <dxgi1_5.h> 
#include <minwindef.h> 
#include <sal.h>  
#include <windef.h>
#include <winnt.h>
#include <wrl\client.h>
#include "CommandQueue.h"
#include "RootSignature.h"

using namespace Microsoft::WRL;

class GPUDescriptorHandle
{
public:

};

class GPUDescriptorAllocator
{
public:
	GPUDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE aType,UINT aNumDescriptorsPerHeap = 256);
	~GPUDescriptorAllocator();

	D3D12_CPU_DESCRIPTOR_HANDLE Allocate(UINT aNumDescriptors = 1);
	void Reset();
};


class GPUSupport
{
public:
	D3D_FEATURE_LEVEL targetFeatureLevel;
	D3D_SHADER_MODEL targetShaderModel;
};


class GPUSwapchain
{
public:
	void Create(HWND hwnd,ComPtr<ID3D12CommandQueue>,UINT Width,UINT Height,UINT bufferCount);
	ComPtr<IDXGISwapChain4> m_SwapChain;
};



enum ePIPELINE_STAGE
{
	PIPELINE_STAGE_INPUTASSEMBLER = 0x1L,
	PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
	PIPELINE_STAGE_GEOMETERY_SHADER = 0x4L,
	PIPELINE_STAGE_RASTERIZER = 0x8L,
	PIPELINE_STAGE_PIXEL_SHADER = 0x10L
} 	inline ePIPELINE_STAGE;

class GPU
{
public:
	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,std::shared_ptr<Texture> outBackBuffer,std::shared_ptr<Texture> outDepthBuffer);
	static bool UnInitialize();
	static void Present(unsigned aSyncInterval = 0);

	static void UpdateBufferResource(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements,size_t elementSize,const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	);

	static void ConfigureInputAssembler(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		D3D_PRIMITIVE_TOPOLOGY topology,
		const D3D12_VERTEX_BUFFER_VIEW& vertView,
		const D3D12_INDEX_BUFFER_VIEW& indexView
	);

	template<typename vertexType = Vertex>
	static bool CreateVertexBuffer(
		D3D12_VERTEX_BUFFER_VIEW& outVertexBufferView,
		ComPtr<ID3D12Resource>& outVxBuffer,
		const std::vector<vertexType>& aVertexList);

	static bool CreateIndexBuffer(D3D12_INDEX_BUFFER_VIEW& outIndexBufferView,ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<unsigned>& aIndexList);

	static bool CreatePixelShader(ComPtr<ID3DBlob>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateVertexShader(ComPtr<ID3DBlob>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC depthStencilDesc);

	static void ResizeDepthBuffer(unsigned width,unsigned height);

	static bool LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName,bool generateMips = true);

	static bool LoadTextureFromMemory(Texture* outTexture,const std::filesystem::path& aName,const BYTE* someImageData,size_t anImageDataSize,const
		D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);

	static void TransitionResource(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		ComPtr<ID3D12Resource> resource,
		D3D12_RESOURCE_STATES beforeState,D3D12_RESOURCE_STATES afterState);

	static void ClearRTV(ComPtr<ID3D12GraphicsCommandList> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT* clearColor);

	static void ClearDepth(ComPtr<ID3D12GraphicsCommandList> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv,FLOAT depth = 1);

	static ComPtr<ID3D12Resource> GetCurrentRenderTargetView();
	static ComPtr<ID3D12Resource> GetCurrentBackBuffer();

	static ComPtr<ID3D12DescriptorHeap>  CreateDescriptorHeap(
		ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	static void UpdateRenderTargetViews(ComPtr<ID3D12Device> device,ComPtr<IDXGISwapChain4> swapChain,ComPtr<ID3D12DescriptorHeap> descriptorHeap);

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device> device,D3D12_COMMAND_LIST_TYPE type);

	static ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device> device,ComPtr<ID3D12CommandAllocator> commandAllocator,D3D12_COMMAND_LIST_TYPE type);

	static ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device> device);

	static HANDLE CreateEventHandle();

	static void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);


	static inline constexpr bool m_useWarpDevice = false;
	static inline constexpr UINT m_FrameCount = 2;
	static inline UINT m_FrameIndex;

	static inline ComPtr<ID3D12Device> m_Device;
	static inline std::unique_ptr<GPUCommandQueue> m_CommandQueue;
	static inline std::unique_ptr<GPUSwapchain> m_Swapchain;
	static inline ComPtr<ID3D12Resource> m_renderTargets[m_FrameCount];
	static inline ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	static inline ComPtr<ID3D12DescriptorHeap> m_SrvHeap;
	static inline UINT m_RtvDescriptorSize;

	static inline uint64_t m_FenceValues[m_FrameCount] = {};

	static inline GPURootSignature m_RootSignature;
	static inline ComPtr<ID3D12PipelineState> m_PipeLineState;


	static inline GPUSupport m_DeviceSupport;
	static inline D3D12_VIEWPORT m_Viewport;
	static inline D3D12_RECT m_ScissorRect;
	static inline std::shared_ptr<Texture> m_BackBuffer;
	static inline std::shared_ptr<Texture> m_DepthBuffer;
	static inline std::unique_ptr<GraphicsMemory> m_GraphicsMemory;


private:
};

template<typename vertexType>
inline bool GPU::CreateVertexBuffer(
	D3D12_VERTEX_BUFFER_VIEW& outvertexBufferView,
	ComPtr<ID3D12Resource>& outVxBuffer,
	const std::vector<vertexType>& aVertexList)
{
	outVxBuffer;
	//const D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(aVertexList));
	//Helpers::ThrowIfFailed(m_Device->CreateCommittedResource(
	//	&heapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&bufferDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&outVxBuffer)));
	//
	//UINT8* pVertexDataBegin;
	//CD3DX12_RANGE readRange(0,0);
	//Helpers::ThrowIfFailed(
	//	outVxBuffer->Map(0,&readRange,reinterpret_cast<void**>(&pVertexDataBegin))
	//);
	//memcpy(pVertexDataBegin,aVertexList.data(),sizeof(aVertexList));
	//outVxBuffer->Unmap(0,nullptr);
	//
	//// Initialize the vertex buffer view.
	//outvertexBufferView.BufferLocation = outVxBuffer->GetGPUVirtualAddress();
	//outvertexBufferView.StrideInBytes = sizeof(vertexType);
	//outvertexBufferView.SizeInBytes = sizeof(aVertexList);

	const GraphicsResource vertexBuffer = m_GraphicsMemory->Allocate(sizeof(vertexType) * aVertexList.size());
	memcpy(vertexBuffer.Memory(),aVertexList.data(),sizeof(vertexType) * aVertexList.size());


	outvertexBufferView.BufferLocation = vertexBuffer.GpuAddress();
	outvertexBufferView.StrideInBytes = sizeof(Vertex);
	outvertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.Size());
	m_CommandQueue->GetCommandList()->IASetVertexBuffers(0,1,&outvertexBufferView);



	/*ComPtr<ID3D12Resource> intermediateVertexBuffer;
	const size_t vxSize = sizeof(vertexType);
	const size_t numVx = aVertexList.size();
	UpdateBufferResource(
		m_CommandQueue->GetCommandList(),
		&outVxBuffer,&intermediateVertexBuffer
		,numVx,vxSize,aVertexList.data()
	);*/
	return true;
}
