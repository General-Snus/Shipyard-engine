#pragma once
#include <d3dx12.h>

class GPU;
class IndexResource;
class VertexResource;
class GpuResource;
class Texture;
class GPUCommandQueue;
class CommandList; 
class GPURootSignature;
class ResourceStateTracker;

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS) - 1)
#define MY_IID_PPV_ARGS IID_PPV_ARGS

enum class eHeapTypes;
enum class ViewType;
enum class ePIPELINE_STAGE;
enum class eRootBindings;


using ID3DBlob = ID3D10Blob;
using DxCommandList = ID3D12GraphicsCommandList10;
using DeviceType = ID3D12Device8;

#define GPUInstance ServiceLocator::Instance().GetService<GPU>()
