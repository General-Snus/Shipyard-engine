#pragma once

using namespace Microsoft::WRL;
class IndexResource;
class VertexResource;
class GpuResource;
class Texture;
class GPUCommandQueue;
class GPURootSignature;
struct IDXGISwapChain4;

enum class	eHeapTypes : int
{
	HEAP_TYPE_CBV_SRV_UAV = 0,
	HEAP_TYPE_SAMPLER,
	HEAP_TYPE_RTV,
	HEAP_TYPE_DSV,
	HEAP_COUNT
};

enum class ViewType
{
	SRV,
	RTV,
	UAV,
	DSV
};

struct HeapHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuPtr;;
	int heapOffset = -1;
};

enum class ePIPELINE_STAGE
{
	PIPELINE_STAGE_INPUTASSEMBLER = 0x1L,
	PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
	PIPELINE_STAGE_GEOMETERY_SHADER = 0x4L,
	PIPELINE_STAGE_RASTERIZER = 0x8L,
	PIPELINE_STAGE_PIXEL_SHADER = 0x10L
};