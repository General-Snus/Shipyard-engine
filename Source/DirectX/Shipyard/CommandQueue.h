#pragma once 
#include "CommandList.h" 
#include "ThreadSafeQueue.h"

class GPUCommandQueue
{
public:
	bool Create(const ComPtr<ID3D12Device>& device,D3D12_COMMAND_LIST_TYPE type);
	std::shared_ptr<CommandList> GetCommandList();

	ComPtr<ID3D12CommandQueue> GetCommandQueue();
	uint64_t ExecuteCommandList(std::shared_ptr<CommandList> commandList);
	uint64_t ExecuteCommandList(const std::vector<std::shared_ptr<CommandList>>& commandLists);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void ProccessInFlightCommandLists();
	void Flush();

	void Wait(const GPUCommandQueue& other);

private:
	using CommandListEntry = std::tuple<uint64_t,std::shared_ptr<CommandList> >;

	ComPtr<ID3D12Device> m_Device;
	D3D12_COMMAND_LIST_TYPE m_CommandListType;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12Fence> m_Fence;
	std::atomic_uint64_t m_FenceValue;


	ThreadSafeQueue<CommandListEntry> m_InFlightCommandLists;
	ThreadSafeQueue<std::shared_ptr<CommandList>> m_AvailableCommandLists;

	// A thread to process in-flight command lists.
	std::thread m_ProcessInFlightCommandListsThread;
	std::atomic_bool m_bProcessInFlightCommandLists;
	std::mutex m_ProcessInFlightCommandListsThreadMutex;
	std::condition_variable m_ProcessInFlightCommandListsThreadCV;

#if USE_NSIGHT_AFTERMATH
	GFSDK_Aftermath_ContextHandle m_hAftermathCommandListContext;
#endif
};
