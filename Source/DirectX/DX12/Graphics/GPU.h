#pragma once
#ifndef incGPU
#define incGPU
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"

#include <array>
#include <DirectX/XTK/Inc/DescriptorHeap.h>
#include <DirectX/XTK/Inc/GraphicsMemory.h>
#include <Tools/Utilities/System/SingletonTemplate.h>

#include "CommandList.h"
#include "CommandQueue.h"
#include "Enums.h"
#include "Gpu_fwd.h"
#include "RootSignature.h"
#include "Texture.h"

class GPUSupport
{
public:
	D3D_FEATURE_LEVEL targetFeatureLevel;
	D3D_SHADER_MODEL  targetShaderModel;
};

class GPUSwapchain
{
public:
	void                    Create(HWND hwnd, ComPtr<ID3D12CommandQueue>, UINT Width, UINT Height, UINT bufferCount);
	void                    Present();
	void                    Resize(Vector2ui resolution);
	ComPtr<IDXGISwapChain4> m_SwapChain;
	DXGI_SWAP_CHAIN_DESC1   m_Desc{};
};

#define GPUInstance ServiceLocator::Instance().GetService<GPU>()

class GPU : public Singleton
{
public:
	friend class CommandList;
	friend class GpuResource;
	bool Initialize(HWND aWindowHandle, bool enableDeviceDebug, uint32_t width, uint32_t height);
	bool UnInitialize();
	void Resize(Vector2ui resolution);
	void Present(unsigned aSyncInterval = 0);

	void UpdateBufferResource(const CommandList& commandList, ID3D12Resource**    pDestinationResource,
	                          ID3D12Resource**   pIntermediateResource, size_t    numElements, size_t elementSize,
	                          const void*        bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	void CopyBuffer(const CommandList& aCommandList, GpuResource&       buffer, size_t numElements, size_t elementSize,
	                const void*        bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	void ConfigureInputAssembler(CommandList&   commandList, D3D_PRIMITIVE_TOPOLOGY topology,
	                             IndexResource& indexResource);

	HeapHandle GetHeapHandle(eHeapTypes type);
	HeapHandle GetHeapHandle(DirectX::DescriptorPile& pile);

	template <typename vertexType>
	static bool CreateVertexBuffer(const std::shared_ptr<CommandList>& commandList, VertexResource& outVxBufferView,
	                               const std::vector<vertexType>&      aVertexList,
	                               CD3DX12_HEAP_PROPERTIES             aHeapProperties = CD3DX12_HEAP_PROPERTIES(
		                               D3D12_HEAP_TYPE_DEFAULT));

	static bool CreateIndexBuffer(const std::shared_ptr<CommandList>& commandList, IndexResource& outIndexResource,
	                              const std::vector<uint32_t>&        aIndexList,
	                              CD3DX12_HEAP_PROPERTIES             aHeapProperties = CD3DX12_HEAP_PROPERTIES(
		                              D3D12_HEAP_TYPE_DEFAULT));

	void ResizeDepthBuffer(unsigned width, unsigned height);

	bool LoadTexture(Texture* outTexture, const std::filesystem::path& aFileName, bool generateMips = true);

	bool LoadTextureFromMemory(Texture* outTexture, const std::filesystem::path& aName, const BYTE* someImageData,
	                           size_t anImageDataSize, bool generateMips = true,
	                           const D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);

	static void TransitionResource(const CommandList&    commandList, const ComPtr<ID3D12Resource>& resource,
	                               D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES         afterState);

	static void ClearRTV(const CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtv,
	                     Vector4f           clearColor = {0, 0, 0, 0});

	void ClearRTV(const CommandList& commandList, Texture* rtv, Vector4f clearColor);

	void ClearRTV(const CommandList& commandList, Texture* rtv, unsigned textureCount, Vector4f clearColor);

	static void ClearRTV(const CommandList& commandList, Texture* rtv, unsigned textureCount = 1);

	static void ClearDepth(const CommandList& commandList, Texture* texture);

	static void ClearDepth(const CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 0);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView();
	Texture&                    GetCurrentBackBuffer();

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		const DeviceType&           device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	void UpdateRenderTargetViews(const DeviceType&                   device, const ComPtr<IDXGISwapChain4>& swapChain,
	                             const ComPtr<ID3D12DescriptorHeap>& descriptorHeap);

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(DeviceType device, D3D12_COMMAND_LIST_TYPE type);
	static CommandList& CreateCommandList(DeviceType device, ComPtr<ID3D12CommandAllocator> commandAllocator,
	                                      D3D12_COMMAND_LIST_TYPE type);
	std::shared_ptr<GPUCommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

	ComPtr<ID3D12Fence> CreateFence();

	static HANDLE CreateEventHandle();

	static void GetHardwareAdapter(_In_ IDXGIFactory4* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
	                               bool                requestHighPerformanceAdapter = false);

#if (USE_NSIGHT_AFTERMATH)
	static void setAftermathEventMarker(const std::string &markerData, bool appManagedMarker);
	static std::string createMarkerStringForFrame(const char *markerString);
#endif
	static constexpr UINT m_FrameCount = 2;
	static constexpr bool m_useWarpDevice = false;
	UINT                  m_FrameIndex;

	DeviceType                       m_Device;
	unsigned                         m_Width;
	unsigned                         m_Height;
	std::shared_ptr<GPUCommandQueue> m_DirectCommandQueue;
	std::shared_ptr<GPUCommandQueue> m_CopyCommandQueue;
	std::shared_ptr<GPUCommandQueue> m_ComputeCommandQueue;

	std::unique_ptr<GPUSwapchain>                            m_Swapchain;
	std::array<ComPtr<ID3D12CommandAllocator>, m_FrameCount> m_CommandAllocators;
	Texture                                                  m_renderTargets[m_FrameCount];

	std::mutex                 lockForTextureLoad;
	uint64_t                   m_FenceValues[m_FrameCount] = {};
	D3D_ROOT_SIGNATURE_VERSION m_FeatureData;

#if (USE_NSIGHT_AFTERMATH)
	UINT64 m_frameCounter;
	GpuCrashTracker::MarkerMap m_markerMap;

	GFSDK_Aftermath_ContextHandle m_hAftermathCommandListContext;
	GpuCrashTracker *m_gpuCrashTracker;
#endif

	GPUSupport                      m_DeviceSupport;
	D3D12_VIEWPORT                  m_Viewport;
	D3D12_RECT                      m_ScissorRect;
	std::shared_ptr<Texture>        m_DepthBuffer;
	std::shared_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;

	std::unique_ptr<DirectX::DescriptorPile> m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_COUNT)];
	std::array<size_t, static_cast<int>(eHeapTypes::HEAP_COUNT)> m_HeapSizes;
};

template <typename vertexType>
bool GPU::CreateVertexBuffer(const std::shared_ptr<CommandList>& commandList, VertexResource& outVxBufferView,
                             const std::vector<vertexType>&      aVertexList,
                             CD3DX12_HEAP_PROPERTIES             aHeapProperties)
{
	commandList->CopyBuffer(outVxBufferView, aVertexList.size(), sizeof(vertexType), aVertexList.data(),
	                        D3D12_RESOURCE_FLAG_NONE,
	                        aHeapProperties);

	return true;
}
#endif
