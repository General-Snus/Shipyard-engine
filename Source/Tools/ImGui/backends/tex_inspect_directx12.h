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
    // DirectX data
    struct ImGui_ImplDX12_Data {
        ID3D12Device* pd3dDevice;
        ID3D12RootSignature* pRootSignature;
        ID3D12PipelineState* pPipelineState;
        DXGI_FORMAT                 RTVFormat;
        ID3D12Resource* pFontTextureResource;
        ID3D12DescriptorHeap* pd3dSrvDescHeap;
        unsigned                        numFramesInFlight;
    };

    bool ImplDX12_Init(ID3D12Device* device,int num_frames_in_flight);
    void ImplDX12_Shutdown();
} 
