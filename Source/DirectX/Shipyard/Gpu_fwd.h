#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class GPU;
class IndexResource;
class VertexResource;
class GpuResource;
class Texture;
class GPUCommandQueue;
class GPURootSignature;
class ResourceStateTracker;

// struct IDXGISwapChain4;
// struct D3D12_CPU_DESCRIPTOR_HANDLE;
// struct D3D12_GPU_DESCRIPTOR_HANDLE;
// struct ID3D10Blob;
// struct ID3D12RootSignature;
// struct ID3D12GraphicsCommandList2;
// struct ID3D12Resource;
struct IDxcLibrary;
struct IDxcCompiler;
struct HeapHandle;
struct IDxcBlob;
// struct D3D12_FEATURE_DATA_FORMAT_SUPPORT;

enum class eHeapTypes;
enum class ViewType;
enum class ePIPELINE_STAGE;
enum class eRootBindings;

typedef ID3D10Blob ID3DBlob;

using DxCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>;
using namespace Microsoft::WRL;
