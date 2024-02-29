#pragma once
#include <Engine/GraphicsEngine/InterOp/GPU.h>
#include <queue> 
#include <wrl/wrappers/corewrappers.h>

class GPUCommandQueue
{
public:
	bool Create(const ComPtr<ID3D12Device>& device,D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	ComPtr<ID3D12CommandQueue> GetCommandQueue();
	uint64_t ExecuteCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();
protected:
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList> CreateCommandList(const ComPtr<ID3D12CommandAllocator>& allocator);


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
	Wrappers::Event m_FenceEvent;
	uint64_t m_FenceValue = 0;

	std::queue<CommandAllocatorEntry> m_CommandAllocatorQueue;
	std::queue<ComPtr<ID3D12GraphicsCommandList>> m_CommandListQueue;
	//ComPtr<ID3D12GraphicsCommandList> m_CommandList; 
	//CommandAllocatorEntry m_Allocator[g_FrameCount];
	//uint64_t g_FrameFenceValues[m_FrameCount] = {};
};
