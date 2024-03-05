#pragma once
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib") 

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

#include <DirectX/CrashHandler/GFSDK_Aftermath.h> 
#include <DirectX/directx/d3d12.h>
#include <DirectX/XTK/DescriptorHeap.h>
#include <DirectX/XTK/GraphicsMemory.h>
#include <dxgi1_6.h>



#include "CommandList.h"
#include "CommandQueue.h"
#include "RootSignature.h"
#include "Texture.h"

class IndexResource;
class VertexResource;
class GpuResource;
class Texture;
namespace DirectX
{
	/*class ResourceUploadBatch;
	class DescriptorHeap;
	class GraphicsMemory;*/
}
class GPUCommandQueue;
struct IDXGISwapChain4;
using namespace Microsoft::WRL;


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
	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,const std::shared_ptr<Texture>& aBackBuffer,const std::shared_ptr<Texture>& aDepthBuffer
		,uint32_t width,uint32_t height);
	static bool UnInitialize();
	static void Present(unsigned aSyncInterval = 0);

	static void UpdateBufferResource(
		const CommandList& commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements,size_t elementSize,const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	);

	static void CopyBuffer(const CommandList& aCommandList,GpuResource& buffer,size_t numElements,size_t elementSize,const void* bufferData,D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	static void ConfigureInputAssembler(
		const CommandList& commandList,
		D3D_PRIMITIVE_TOPOLOGY topology,
		const D3D12_VERTEX_BUFFER_VIEW& vertView,
		const D3D12_INDEX_BUFFER_VIEW& indexView
	);

	template<typename vertexType>
	static bool CreateVertexBuffer(
		const std::shared_ptr<CommandList>& commandList,
		VertexResource& outVxBufferView,
		const std::vector<vertexType>& aVertexList);

	static bool CreateIndexBuffer(
		const std::shared_ptr<CommandList>& commandList,
		IndexResource& outIndexResource,
		const std::vector<unsigned>& aIndexList
	);

	static bool CreatePixelShader(ComPtr<ID3DBlob>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateVertexShader(ComPtr<ID3DBlob>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateDepthStencil(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);

	static void ResizeDepthBuffer(unsigned width,unsigned height);

	static bool LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName,bool generateMips = true);

	static bool LoadTextureFromMemory(Texture* outTexture,const std::filesystem::path& aName,const BYTE* someImageData,size_t anImageDataSize,const
		D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);

	static void TransitionResource(
		const CommandList& commandList,
		const ComPtr<ID3D12Resource>& resource,
		D3D12_RESOURCE_STATES beforeState,D3D12_RESOURCE_STATES afterState);

	static void ClearRTV(const CommandList& commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT* clearColor);

	static void ClearDepth(const CommandList& commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv,FLOAT depth = 1);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView();
	static Texture& GetCurrentBackBuffer();

	static ComPtr<ID3D12DescriptorHeap>  CreateDescriptorHeap(
		const ComPtr<ID3D12Device>& device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	static void UpdateRenderTargetViews(const ComPtr<ID3D12Device>& device,const ComPtr<IDXGISwapChain4>& swapChain,const ComPtr<ID3D12DescriptorHeap>& descriptorHeap);

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device> device,D3D12_COMMAND_LIST_TYPE type);
	static CommandList& CreateCommandList(ComPtr<ID3D12Device> device,ComPtr<ID3D12CommandAllocator> commandAllocator,D3D12_COMMAND_LIST_TYPE type);
	static std::shared_ptr<GPUCommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

	static ComPtr<ID3D12Fence> CreateFence(const ComPtr<ID3D12Device>& device);

	static HANDLE CreateEventHandle();

	static void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);

#if (USE_NSIGHT_AFTERMATH)
	static void  setAftermathEventMarker(const std::string& markerData,bool appManagedMarker);
	static std::string  createMarkerStringForFrame(const char* markerString);
#endif
	static inline constexpr UINT m_FrameCount = 2;
	static inline constexpr bool m_useWarpDevice = false;
	static inline UINT m_FrameIndex;

	static inline ComPtr<ID3D12Device> m_Device;
	static inline std::shared_ptr<GPUCommandQueue> m_DirectCommandQueue;
	static inline std::shared_ptr<GPUCommandQueue> m_CopyCommandQueue;
	static inline std::shared_ptr<GPUCommandQueue> m_ComputeCommandQueue;

	static inline std::unique_ptr<GPUSwapchain> m_Swapchain;
	static inline ComPtr<ID3D12CommandAllocator> m_CommandAllocators[m_FrameCount];
	static inline Texture m_renderTargets[m_FrameCount];

	static inline ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	static inline ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
	static inline ComPtr<ID3D12DescriptorHeap> m_SrvHeap;
	static inline UINT m_RtvDescriptorSize;

	static inline uint64_t m_FenceValues[m_FrameCount] = {};

	static inline GPURootSignature m_RootSignature;
	static inline ComPtr<ID3D12PipelineState> m_PipeLineState;


#if  (USE_NSIGHT_AFTERMATH)
	// App-managed marker functionality
	static inline UINT64 m_frameCounter;
	static inline GpuCrashTracker::MarkerMap m_markerMap;

	// Nsight Aftermath instrumentation
	static inline GFSDK_Aftermath_ContextHandle m_hAftermathCommandListContext;
	static inline GpuCrashTracker* m_gpuCrashTracker;
#endif


	static inline GPUSupport m_DeviceSupport;
	static inline D3D12_VIEWPORT m_Viewport;
	static inline D3D12_RECT m_ScissorRect;
	static inline std::shared_ptr<Texture> m_BackBuffer;
	static inline std::shared_ptr<Texture> m_DepthBuffer;
	static inline std::unique_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;
	static inline std::unique_ptr<DirectX::DescriptorHeap> m_ResourceDescriptors;
	static inline ComPtr<ID3D12DescriptorHeap> guiDescriptorHeap;;

private:
};

template<typename vertexType>
inline bool GPU::CreateVertexBuffer(
	const std::shared_ptr<CommandList>& commandList,VertexResource& outVxBufferView,
	const std::vector<vertexType>& aVertexList)
{
	commandList->CopyBuffer(
		outVxBufferView,
		aVertexList.size(),
		sizeof(vertexType),
		aVertexList.data()
	);

	return true;
}


