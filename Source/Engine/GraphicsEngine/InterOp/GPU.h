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

class GPURootParameter
{
	friend class RootSignature;
public:

	GPURootParameter()
	{
		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	~GPURootParameter()
	{
		Clear();
	}

	void Clear()
	{
		if (m_RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[] m_RootParam.DescriptorTable.pDescriptorRanges;

		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	void InitAsConstants(UINT Register,UINT NumDwords,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Constants.Num32BitValues = NumDwords;
		m_RootParam.Constants.ShaderRegister = Register;
		m_RootParam.Constants.RegisterSpace = Space;
	}

	void InitAsConstantBuffer(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void InitAsBufferSRV(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void InitAsBufferUAV(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type,UINT Register,UINT Count,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
	{
		InitAsDescriptorTable(1,Visibility);
		SetTableRange(0,Type,Register,Count,Space);
	}

	void InitAsDescriptorTable(UINT RangeCount,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
		m_RootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
	}

	void SetTableRange(UINT RangeIndex,D3D12_DESCRIPTOR_RANGE_TYPE Type,UINT Register,UINT Count,UINT Space = 0)
	{
		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_RootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
		range->RangeType = Type;
		range->NumDescriptors = Count;
		range->BaseShaderRegister = Register;
		range->RegisterSpace = Space;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	const D3D12_ROOT_PARAMETER& operator() (void) const { return m_RootParam; }


protected:

	D3D12_ROOT_PARAMETER m_RootParam;
};

class GPURootSignature
{
	friend class GPU;
public:

	GPURootSignature(UINT NumRootParams = 0,UINT NumStaticSamplers = 0) : m_Finalized(FALSE),m_NumParameters(NumRootParams)
	{
		Reset(NumRootParams,NumStaticSamplers);
	}

	//take your time to learn this when not cracked
	void RegisterSampler(UINT Register,const D3D12_SAMPLER_DESC& nonStaticSamplerDesc,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	void Reset(UINT NumRootParams,UINT NumStaticSamplers = 0);


	GPURootParameter& operator[] (size_t EntryIndex)
	{
		assert(EntryIndex < m_NumParameters);
		return m_ParamArray.get()[EntryIndex];
	}

	const GPURootParameter& operator[] (size_t EntryIndex) const
	{
		assert(EntryIndex < m_NumParameters);
		return m_ParamArray.get()[EntryIndex];
	}
	//take your time to learn this when not cracked
	void Finalize(const std::wstring& name,D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;
	BOOL m_Finalized;
	UINT m_NumParameters;
	UINT m_NumSamplers;
	UINT m_NumInitializedStaticSamplers;
	uint32_t m_DescriptorTableBitMap;		// One bit is set for root parameters that are non-sampler descriptor tables
	uint32_t m_SamplerTableBitMap;			// One bit is set for root parameters that are sampler descriptor tables
	uint32_t m_DescriptorTableSize[16];		// Non-sampler descriptor tables need to know their descriptor count
	std::unique_ptr<GPURootParameter[]> m_ParamArray;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> m_SamplerArray;


};

enum ePIPELINE_STAGE
{
	PIPELINE_STAGE_INPUTASSEMBLER = 0x1L,
	PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
	PIPELINE_STAGE_GEOMETERY_SHADER = 0x4L,
	PIPELINE_STAGE_RASTERIZER = 0x8L,
	PIPELINE_STAGE_PIXEL_SHADER = 0x10L
} 	inline ePIPELINE_STAGE;

enum eRootBindings
{
	MeshConstants,
	MaterialConstants,
	MaterialSRVs,
	MaterialSamplers,
	CommonSRVs,
	CommonCBV,
	SkinMatrices,

	count
};
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
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	);

	static void ConfigureInputAssembler(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		D3D_PRIMITIVE_TOPOLOGY topology,
		const D3D12_VERTEX_BUFFER_VIEW& vertView,
		const D3D12_INDEX_BUFFER_VIEW& indexView
	);

	template <typename vertexType>
	static bool CreateVertexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<vertexType>& aVertexList);
	static bool CreateIndexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<unsigned>& aIndexList);

	static bool CreatePixelShader(ComPtr<ID3DBlob>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateVertexShader(ComPtr<ID3DBlob>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize,UINT CompileFLags = 0);

	static bool CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC depthStencilDesc);


	static void ResizeDepthBuffer(unsigned width,unsigned height);

	static bool LoadTexture(Texture* outTexture,const std::filesystem::path& aFileName);

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

	static inline GPURootSignature m_RootSignature;
	static inline ComPtr<ID3D12PipelineState> m_PipeLineState;


	static inline GPUSupport m_DeviceSupport;
	static inline D3D12_VIEWPORT m_Viewport;
	static inline D3D12_RECT m_ScissorRect;
	static inline Texture* m_BackBuffer;
	static inline Texture* m_DepthBuffer;

private:
};

template<typename vertexType>
inline bool GPU::CreateVertexBuffer(ComPtr<ID3D12Resource>& outVxBuffer,const std::vector<vertexType>& aVertexList)
{
	ComPtr<ID3D12Resource> intermediateVertexBuffer;
	const size_t vxSize = sizeof(vertexType);
	const size_t numVx = aVertexList.size();
	UpdateBufferResource(
		m_CommandQueue->GetCommandList(),
		&outVxBuffer,&intermediateVertexBuffer
		,numVx,vxSize,aVertexList.data()
	);
	return true;
}
