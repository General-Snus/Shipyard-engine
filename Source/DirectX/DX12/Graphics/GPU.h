#pragma once
#ifndef incGPU
#define incGPU
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED  
#include "Enums.h" 
#include "Tools/Utilities/LinearAlgebra/Vector2.hpp"
#include "Tools/Utilities/LinearAlgebra/Vector4.hpp"
#include "Tools/Utilities/System/ServiceLocator.h" 
#include <DirectX/DX12/Graphics/Resources/Texture.h>
#include <DirectX/DX12/Graphics/Gpu_fwd.h>
#include <DirectX/XTK/Inc/DescriptorHeap.h>
#include <DirectX/XTK/Inc/GraphicsMemory.h>
#include <Tools/Utilities/System/SingletonTemplate.h> 
#include <array> 
#include <d3d12.h>
#include <dxgi1_6.h> 

class GPUSupport
{
public:
	D3D_FEATURE_LEVEL targetFeatureLevel;
	D3D_SHADER_MODEL  targetShaderModel;
};

class GPUSwapchain
{
public:
	void                    Create(HWND hwnd, Ref<ID3D12CommandQueue>, UINT Width, UINT Height, UINT bufferCount);
	void                    Present() const;
	void                    Resize(Vector2ui resolution) const;
	Ref<IDXGISwapChain4> m_SwapChain{};
	DXGI_SWAP_CHAIN_DESC1   m_Desc{};
};


class GPU : public Singleton
{
public:
	friend class CommandList;
	friend class GpuResource;
	bool Initialize(HWND aWindowHandle, bool enableDeviceDebug, uint32_t width, uint32_t height);
	bool UnInitialize();
	void Resize(Vector2ui resolution);
	void Present(unsigned aSyncInterval = 0);

	void UpdateBufferResource(const CommandList& commandList, ID3D12Resource** pDestinationResource,
							  ID3D12Resource** pIntermediateResource, size_t    numElements, size_t elementSize,
							  const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) const;

	void CopyBuffer(const CommandList& aCommandList, GpuResource& buffer, size_t numElements, size_t elementSize,
					const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	HeapHandle GetHeapHandle(eHeapTypes type);
	HeapHandle GetHeapHandle(DirectX::DescriptorPile& pile);




	void ResizeDepthBuffer(unsigned width, unsigned height);

	bool LoadTexture(Texture* outTexture, const std::filesystem::path& aFileName, bool generateMips = true);

	bool LoadTextureFromMemory(Texture* outTexture, const std::filesystem::path& aName, const BYTE* someImageData,
							   size_t anImageDataSize, bool generateMips = true,
							   const D3D12_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);

	static void TransitionResource(const CommandList& commandList, const Ref<ID3D12Resource>& resource,
								   D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

	static void ClearRTV(const CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtv,
						 Vector4f           clearColor = { 0,0,0,0 });

	void ClearRTV(const CommandList& commandList, Texture* rtv, Vector4f clearColor);

	void ClearRTV(const CommandList& commandList, Texture* rtv, unsigned textureCount, Vector4f clearColor);

	static void ClearRTV(const CommandList& commandList, Texture* rtv, unsigned textureCount = 1);

	static void ClearDepth(const CommandList& commandList, Texture* texture);

	static void ClearDepth(const CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 0);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView();
	Texture& GetCurrentBackBuffer();

	Ref<ID3D12DescriptorHeap> CreateDescriptorHeap(
		const Ref<DeviceType>& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	void UpdateRenderTargetViews(const Ref<DeviceType>& device, const Ref<IDXGISwapChain4>& swapChain);

	static Ref<ID3D12CommandAllocator> CreateCommandAllocator(Ref<DeviceType> device, D3D12_COMMAND_LIST_TYPE type);

	static CommandList& CreateCommandList(Ref<DeviceType> device, Ref<ID3D12CommandAllocator> commandAllocator,
										  D3D12_COMMAND_LIST_TYPE type);
	std::shared_ptr<GPUCommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const;

	Ref<ID3D12Fence> CreateFence() const;

	static HANDLE CreateEventHandle();

	static void GetHardwareAdapter(_In_ IDXGIFactory4* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
								   bool                requestHighPerformanceAdapter = false);

#if (USE_NSIGHT_AFTERMATH)
	static void setAftermathEventMarker(const std::string& markerData, bool appManagedMarker);
	static std::string createMarkerStringForFrame(const char* markerString);
#endif
	static constexpr UINT m_FrameCount = 2;
	static constexpr bool m_useWarpDevice = false;
	UINT                  m_FrameIndex;

	Ref<DeviceType>                       m_Device{};
	unsigned                         m_Width{};
	unsigned                         m_Height{};
	std::shared_ptr<GPUCommandQueue> m_DirectCommandQueue{};
	std::shared_ptr<GPUCommandQueue> m_CopyCommandQueue{};
	std::shared_ptr<GPUCommandQueue> m_ComputeCommandQueue{};

	std::unique_ptr<GPUSwapchain>                            m_Swapchain;
	std::array<Ref<ID3D12CommandAllocator>, m_FrameCount> m_CommandAllocators;
	std::array<Texture, m_FrameCount> m_renderTargets;

	std::mutex                 lockForTextureLoad{};
	std::array<uint64_t, m_FrameCount> m_FenceValues = {};
	D3D_ROOT_SIGNATURE_VERSION m_FeatureData{};

#if (USE_NSIGHT_AFTERMATH)
	UINT64 m_frameCounter;
	GpuCrashTracker::MarkerMap m_markerMap;

	GFSDK_Aftermath_ContextHandle m_hAftermathCommandListContext;
	GpuCrashTracker* m_gpuCrashTracker;
#endif

	GPUSupport                      m_DeviceSupport{};
	D3D12_VIEWPORT                  m_Viewport{};
	D3D12_RECT                      m_ScissorRect{};
	std::shared_ptr<Texture>        m_DepthBuffer{};
	std::shared_ptr<DirectX::GraphicsMemory> m_GraphicsMemory{};

	std::array<std::unique_ptr<DirectX::DescriptorPile>, static_cast<int>(eHeapTypes::HEAP_COUNT)> m_ResourceDescriptors{};
	std::array<size_t, static_cast<int>(eHeapTypes::HEAP_COUNT)> m_HeapSizes{};
};

#endif
