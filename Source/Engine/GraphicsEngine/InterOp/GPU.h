#pragma once
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include <DirectX\directx\d3d12.h> 
#include <dxgi1_6.h> 
#include <Engine/GraphicsEngine/Rendering/Texture.h>
#include <filesystem>
#include <queue>

using namespace DirectX;
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

class GPUCommandQueue
{
public:
	bool Create(ComPtr<ID3D12Device> device,D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	ComPtr<ID3D12CommandQueue> GetCommandQueue();
	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> commandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

protected:
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12CommandAllocator> allocator);


private:
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	D3D12_COMMAND_LIST_TYPE m_CommandListType;
	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_FenceEvent;
	uint64_t m_FenceValue = 0;

	std::queue<CommandAllocatorEntry> m_CommandAllocatorQueue;
	std::queue<ComPtr<ID3D12GraphicsCommandList>> m_CommandListQueue;

	//ComPtr<ID3D12CommandAllocator> m_Allocator[m_FrameCount];
	//uint64_t g_FrameFenceValues[m_FrameCount] = {};
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
	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,Texture* outBackBuffer,Texture* outDepthBuffer);
	static bool UnInitialize();
	static void Present(unsigned aSyncInterval = 0);










	static void UpdateBufferResource(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements,size_t elementSize,const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	template <typename vertexType>
	static void CreateVertexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<vertexType>& aVertexList);
	static void CreateIndexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<unsigned>& aIndexList);

	static bool CreatePixelShader(ComPtr<ID3DBlob>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateVertexShader(ComPtr<ID3DBlob>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC depthStencilDesc);

	static bool LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName);

	static bool LoadTextureFromMemory(Texture* outTexture,const std::filesystem::path& aName,const BYTE* someImageData,size_t anImageDataSize,const
		D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);


	static ComPtr<ID3D12DescriptorHeap>  CreateDescriptorHeap(ComPtr<ID3D12Device> device,D3D12_DESCRIPTOR_HEAP_TYPE type,uint32_t numDescriptors);
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
	static inline constexpr UINT m_FrameCount = 3;
	static inline UINT m_FrameIndex;

	static inline ComPtr<ID3D12Device> m_Device;
	static inline std::unique_ptr<GPUCommandQueue> m_CommandQueue;
	static inline std::unique_ptr<GPUSwapchain> m_Swapchain;
	static inline ComPtr<ID3D12Resource> m_renderTargets[m_FrameCount];
	static inline ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	static inline UINT m_RtvDescriptorSize;

	static inline uint64_t m_FenceValues[m_FrameCount] = {};

	static inline ComPtr<ID3D12RootSignature> m_RootSignature;
	static inline ComPtr<ID3D12PipelineState> m_PipeLineState;



	static inline GPUSupport m_DeviceSupport;
	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

private:
};

template<typename vertexType>
inline void GPU::CreateVertexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<vertexType>& aVertexList)
{
	ComPtr<ID3D12Resource> intermediateVertexBuffer;
	const size_t vxSize = sizeof(vertexType);
	const size_t numVx = aVertexList.size();
	return UpdateBufferResource(
		m_CommandQueue->GetCommandList(),
		&outVxBuffer,&intermediateVertexBuffer
		,numVx,vxSize,aVertexList.data()
	);
}
