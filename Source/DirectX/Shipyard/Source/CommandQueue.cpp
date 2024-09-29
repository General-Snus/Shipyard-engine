#include "DirectX/DirectXHeader.pch.h"

#include "../CommandQueue.h"
#include "../GPU.h"
#include "DirectX/Shipyard/ResourceStateTracker.h"
#include <DirectX/CrashHandler/NsightAftermathGpuCrashTracker.h>

bool GPUCommandQueue::Create(const DeviceType &device, D3D12_COMMAND_LIST_TYPE type)
{
    m_Device = device;
    m_CommandListType = type;
    m_FenceValue = 0;
    m_bProcessInFlightCommandLists = true;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    Helpers::ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)));
    Helpers::ThrowIfFailed(device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

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

    m_ProcessInFlightCommandListsThread = std::jthread(&GPUCommandQueue::ProccessInFlightCommandLists, this);
    return true;
}

uint64_t GPUCommandQueue::Signal()
{
    OPTICK_GPU_EVENT("Signal");
    uint64_t fenceValueForSignal = ++m_FenceValue;
    Helpers::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValueForSignal));
    return fenceValueForSignal;
}

bool GPUCommandQueue::IsFenceComplete(uint64_t fenceValue)
{
    return m_Fence->GetCompletedValue() >= fenceValue;
}

void GPUCommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
    if (!IsFenceComplete(fenceValue))
    {
        auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(event && "Failed to create fence event handle.");

        m_Fence->SetEventOnCompletion(fenceValue, event);
        ::WaitForSingleObject(event, DWORD_MAX);

        ::CloseHandle(event);
    }
}

std::shared_ptr<CommandList> GPUCommandQueue::GetCommandList(const std::wstring &name)
{
    OPTICK_EVENT();
    std::shared_ptr<CommandList> commandList;

    if (!m_AvailableCommandLists.Empty())
    {
        m_AvailableCommandLists.TryPop(commandList);
    }
    else
    {
        commandList = std::make_shared<CommandList>(m_Device, m_CommandListType, name);
    }

#if (USE_NSIGHT_AFTERMATH)
    // Create an Nsight Aftermath context handle for setting Aftermath event markers in this command list.
    AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_DX12_CreateContextHandle(commandList->GetGraphicsCommandList().Get(),
                                                                   &m_hAftermathCommandListContext));
#endif

    return commandList;
}

ComPtr<ID3D12CommandQueue> GPUCommandQueue::GetCommandQueue()
{
    return m_CommandQueue;
}

uint64_t GPUCommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> commandList)
{
    return ExecuteCommandList(std::vector<std::shared_ptr<CommandList>>({commandList}));
}

uint64_t GPUCommandQueue::ExecuteCommandList(const std::vector<std::shared_ptr<CommandList>> &commandLists)
{
    OPTICK_GPU_EVENT("ExecuteCommandList");
    ResourceStateTracker::Lock();

    // Command lists that need to put back on the command list queue.
    std::vector<std::shared_ptr<CommandList>> toBeQueued;
    toBeQueued.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

    // Generate mips command lists.
    std::vector<std::shared_ptr<CommandList>> generateMipsCommandLists;
    generateMipsCommandLists.reserve(commandLists.size());

    // Command lists that need to be executed.
    std::vector<ID3D12CommandList *> d3d12CommandLists;
    d3d12CommandLists.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

    for (auto commandList : commandLists)
    {
        auto pendingCommandList = GetCommandList();
        bool hasPendingBarriers = commandList->Close(*pendingCommandList);
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

        // auto generateMipsCommandList = commandList->GetGenerateMipsCommandList();
        // if (generateMipsCommandList)
        //{
        //	generateMipsCommandLists.push_back(generateMipsCommandList);
        // }
    }

    UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());
    OPTICK_GPU_EVENT("ListExecution");
    m_CommandQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
    uint64_t fenceValue = Signal();

    ResourceStateTracker::Unlock();

    // Queue command lists for reuse.
    for (auto commandList : toBeQueued)
    {
        m_InFlightCommandLists.Push({fenceValue, commandList});
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

void GPUCommandQueue::ProccessInFlightCommandLists()
{
    OPTICK_THREAD("ProccessInFlightCommandLists");
    OPTICK_GPU_EVENT("ProccessInFlightCommandLists");
    std::unique_lock lock(m_ProcessInFlightCommandListsThreadMutex, std::defer_lock);

    while (m_bProcessInFlightCommandLists)
    {
        CommandListEntry commandListEntry;

        lock.lock();
        while (m_InFlightCommandLists.TryPop(commandListEntry))
        {
            auto fenceValue = std::get<0>(commandListEntry);
            auto commandList = std::get<1>(commandListEntry);

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
    std::unique_lock lock(m_ProcessInFlightCommandListsThreadMutex);
    m_ProcessInFlightCommandListsThreadCV.wait(lock, [this] { return m_InFlightCommandLists.Empty(); });

    // In case the command queue was signaled directly
    // using the CommandQueue::Signal method then the
    // fence value of the command queue might be higher than the fence
    // value of any of the executed command lists.
    WaitForFenceValue(m_FenceValue);
}

void GPUCommandQueue::Wait(const GPUCommandQueue &other) const
{
    m_CommandQueue->Wait(other.m_Fence.Get(), other.m_FenceValue);
}

void GPUCommandQueue::Destroy()
{
    Flush();
    m_bProcessInFlightCommandLists = false;
}
