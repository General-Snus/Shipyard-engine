#pragma once
#include <d3dx12.h>
#include <concepts> 
#include <xtr1common> 

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

template <typename U>
concept D3D12ObjectLike =
std::derived_from<std::remove_pointer_t<std::remove_cvref_t<U>>, ID3D12Object>;

template <typename U>
concept RefToD3D12Object =
	requires { typename std::remove_cvref_t<U>::InterfaceType; }&&
std::derived_from<typename std::remove_cvref_t<U>::InterfaceType, ID3D12Object>;

template <typename T>
concept IsResource = D3D12ObjectLike<T> || RefToD3D12Object<T>;

template <typename T>
concept D12Resource = requires(T t) { { GetResource(t) }-> IsResource; };

template <typename T>
constexpr auto GetResource(T&& t)
{
	if constexpr (IsResource<T>)
	{
		return std::forward<T>(t);
	}
	else if constexpr (requires { t->Resource(); })
	{
		return t->Resource();
	}
	else
	{
		return t.Resource();
	}
}
