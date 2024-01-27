#pragma once
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include <DirectX\directx\d3d12.h> 
#include <dxgi1_6.h> 
#include  <Rendering/Texture.h>

class GPUSupport
{

};

class GPUCommandQueue
{
public:
	D3D12_COMMAND_QUEUE_DESC m_CommandQueueDesc;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
};

class GPUSwapchain
{
public:
	//DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc;
	ComPtr<IDXGISwapChain> m_SwapChain;
};

using namespace Microsoft::WRL;
using namespace DirectX;
class GPU
{
public:
	struct DeviceSize
	{
		unsigned Width;
		unsigned Height;
	};

	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,Texture* outBackBuffer,Texture* outDepthBuffer);
	static bool UnInitialize();
	static void Present(unsigned aSyncInterval = 0);
private:
	static void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);


	static inline  bool m_useWarpDevice;
	static inline ComPtr<ID3D12Device> m_Device;
	//static inline ComPtr<ID3D12DeviceFactory> m_Context;
	static inline ComPtr<ID3D12CommandAllocator> m_Allocator;
	std::unique_ptr<GPUSwapchain> m_Swapchain;
	GPUSupport m_DeviceSupport;
};

