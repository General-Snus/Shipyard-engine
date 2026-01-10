#include "DirectXHeader.pch.h"

#include "../CommandQueue.h"
#include <CrashHandler/NsightAftermathGpuCrashTracker.h> 
#include "Graphics/ResourceStateTracker.h"
#include "Graphics/CommandList.h"

bool GPUCommandQueue::Create(const Ref<DeviceType>& device, D3D12_COMMAND_LIST_TYPE type)
{
	OPTICK_EVENT();
	m_Device = device;
	m_CommandListType = type;
	m_FenceValue = 0;
	m_bProcessInFlightCommandLists = true;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	Helpers::ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	Helpers::ThrowIfFailed(device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));

	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_COPY:
		m_CommandQueue->SetName(L"Copy Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		m_CommandQueue->SetName(L"Compute Command Queue");
		break;
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		m_CommandQueue->SetName(L"Direct Command Queue");
		break;
	default:
		assert(false && "Invalid command list type.");
		break;
	}

	m_ProcessInFlightCommandListsThread = std::jthread([this](std::stop_token s)
	{
		this->ProccessInFlightCommandLists(s);
	});
	return true;
}

uint64_t GPUCommandQueue::Signal()
{
	OPTICK_EVENT("Signal");
	const uint64_t fenceValueForSignal = ++m_FenceValue;
	Helpers::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValueForSignal));
	return fenceValueForSignal;
}

bool GPUCommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	OPTICK_EVENT();
	return m_Fence->GetCompletedValue() >= fenceValue;
}

void GPUCommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	OPTICK_EVENT("WaitForFenceValue");
	if (!IsFenceComplete(fenceValue))
	{
		const auto event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(event && "Failed to create fence event handle.");

		m_Fence->SetEventOnCompletion(fenceValue, event);
		WaitForSingleObject(event, DWORD_MAX);

		CloseHandle(event);
	}
}

std::shared_ptr<CommandList> GPUCommandQueue::GetCommandList(const std::wstring& name)
{
	OPTICK_EVENT();
	std::shared_ptr<CommandList> commandList;

	if (!m_AvailableCommandLists.Empty())
	{
		m_AvailableCommandLists.TryPop(commandList);
	}
	else
	{
		LOGGER.Warn("Saturation warning, get command list created new");
		commandList = std::make_shared<CommandList>(m_Device, m_CommandListType, name);
	}

#if (USE_NSIGHT_AFTERMATH)
	// Create an Nsight Aftermath context handle for setting Aftermath event markers in this command list.
	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_DX12_CreateContextHandle(commandList->GetGraphicsCommandList().Get(),
		&m_hAftermathCommandListContext));
#endif

	return commandList;
}

Ref<ID3D12CommandQueue> GPUCommandQueue::GetCommandQueue()
{
	return m_CommandQueue;
}

uint64_t GPUCommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> commandList)
{
	return ExecuteCommandList(std::vector({ commandList }));
}

uint64_t GPUCommandQueue::ExecuteCommandList(const std::vector<std::shared_ptr<CommandList>>& commandLists)
{
	OPTICK_EVENT("ExecuteCommandList");
	ResourceStateTracker::Lock();

	// Command lists that need to put back on the command list queue.
	std::vector<std::shared_ptr<CommandList>> toBeQueued;
	toBeQueued.clear();

	std::vector<ID3D12CommandList*> d3d12CommandLists;
	d3d12CommandLists.clear();

	for (auto& commandList : commandLists)
	{
		auto       pendingCommandList = GetCommandList();
		const bool hasPendingBarriers = commandList->Close(*pendingCommandList);
		pendingCommandList->Close();

		// If there are no pending barriers on the pending command list, there is no reason to
		// execute an empty command list on the command queue.
		if (hasPendingBarriers)
		{
			d3d12CommandLists.push_back(pendingCommandList->GetGraphicsCommandList().Get());
		}
		d3d12CommandLists.push_back(commandList->GetGraphicsCommandList().Get());

		toBeQueued.push_back(pendingCommandList);
		toBeQueued.push_back(commandList);
	}

	const UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());
	OPTICK_EVENT("ListExecution");
	m_CommandQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
	 
	uint64_t fenceValue = Signal();

	ResourceStateTracker::Unlock();

	// Queue command lists for reuse.
	for (auto& commandList : toBeQueued)
	{
		m_InFlightCommandLists.Push({ fenceValue, commandList });
	}

	// If there are any command lists that generate mips then execute those
	// after the initial resource command lists have finished.
	// if (generateMipsCommandLists.size() > 0)
	//{
	//	auto computeQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	//	computeQueue->Wait(*this);
	//	computeQueue->ExecuteCommandLists(generateMipsCommandLists);
	//}

	return fenceValue;
}

void GPUCommandQueue::ProccessInFlightCommandLists(std::stop_token stop_token)
{
	OPTICK_THREAD("ProccessInFlightCommandLists");
	std::unique_lock lock(m_ProcessInFlightCommandListsThreadMutex, std::defer_lock);
	while (m_bProcessInFlightCommandLists || !stop_token.stop_requested())
	{

		CommandListEntry commandListEntry;

		lock.lock();
		while (m_InFlightCommandLists.TryPop(commandListEntry))
		{
			OPTICK_EVENT("poppedList");
			const auto fenceValue = std::get<0>(commandListEntry);
			const auto& commandList = std::get<1>(commandListEntry);

			WaitForFenceValue(fenceValue);

			commandList->Reset();

			m_AvailableCommandLists.Push(commandList);
		}
		lock.unlock();
		m_ProcessInFlightCommandListsThreadCV.notify_one();

		std::this_thread::yield();
	}
}

void GPUCommandQueue::Flush()
{
	OPTICK_EVENT();
	std::unique_lock lock(m_ProcessInFlightCommandListsThreadMutex);
	m_ProcessInFlightCommandListsThreadCV.wait(lock, [this] { return m_InFlightCommandLists.Empty(); });

	// In case the command queue was signaled directly
	// using the CommandQueue::Signal method then the
	// fence value of the command queue might be higher than the fence
	// value of any of the executed command lists.
	WaitForFenceValue(m_FenceValue);
}

void GPUCommandQueue::Wait(const GPUCommandQueue& other) const
{
	OPTICK_EVENT();
	m_CommandQueue->Wait(other.m_Fence.Get(), other.m_FenceValue);
}

void GPUCommandQueue::Destroy()
{
	OPTICK_EVENT();
	Flush();
	m_ProcessInFlightCommandListsThread.join();
	m_bProcessInFlightCommandLists = false;
}
