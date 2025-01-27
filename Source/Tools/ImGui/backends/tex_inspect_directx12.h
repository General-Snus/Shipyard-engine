#pragma once
#include <dxgiformat.h> // DXGI_FORMAT

struct ID3D12Device;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct ID3D12PipelineState;
struct D3D12_GPU_DESCRIPTOR_HANDLE; 
struct ID3D12Resource;
struct ID3D12RootSignature;

namespace ImGuiTexInspect {
   
    bool ImplDX12_Init();
    void ImplDX12_Shutdown();
} 
