#include "../CommandQueue.h"
#include "GraphicsEngine.pch.h"

ComPtr<ID3D12CommandAllocator> GPUCommandQueue::CreateCommandAllocator()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	Helpers::ThrowIfFailed(m_Device->CreateCommandAllocator(m_CommandListType,IID_PPV_ARGS(&commandAllocator)));

	return commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList> GPUCommandQueue::CreateCommandList(const ComPtr<ID3D12CommandAllocator>& allocator)
{
	ComPtr<ID3D12GraphicsCommandList> commandList;
	Helpers::ThrowIfFailed(m_Device->CreateCommandList(0,m_CommandListType,allocator.Get(),nullptr,IID_PPV_ARGS(&commandList)));

	//Helpers::ThrowIfFailed(commandList->Close());

	return commandList;
}

D3D12_CPU_DESCRIPTOR_HANDLE GPUDescriptorAllocator::Allocate(UINT aNumDescriptors)
{
	aNumDescriptors;
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

bool GPUCommandQueue::Create(const ComPtr<ID3D12Device>& device,D3D12_COMMAND_LIST_TYPE type)
{
	m_Device = device;
	m_FenceValue = 0;
	m_CommandListType = type;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	//desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	//desc.NodeMask = 0;

	Helpers::ThrowIfFailed(m_Device->CreateCommandQueue(&desc,IID_PPV_ARGS(&m_CommandQueue)));
	Helpers::ThrowIfFailed(m_Device->CreateFence(m_FenceValue,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&m_Fence)));

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
	}

	m_FenceEvent.Attach(CreateEventEx(nullptr,nullptr,0,EVENT_MODIFY_STATE | SYNCHRONIZE));
	if (!m_FenceEvent.IsValid())
	{
		throw std::system_error(std::error_code(static_cast<int>(GetLastError()),std::system_category()),"CreateEventEx");
	}
	return true;
}

uint64_t GPUCommandQueue::Signal()
{
	uint64_t fenceValueForSignal = ++m_FenceValue;
	Helpers::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(),fenceValueForSignal));

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
		auto event = ::CreateEvent(nullptr,FALSE,FALSE,nullptr);
		if (event)
		{
			m_Fence->SetEventOnCompletion(fenceValue,event);
			std::ignore = WaitForSingleObjectEx(event,INFINITE,FALSE);

			//::CloseHandle(event);
		}
	}
}

ComPtr<ID3D12GraphicsCommandList> GPUCommandQueue::GetCommandList()
{
	ComPtr<ID3D12GraphicsCommandList>  commandList;
	ComPtr<ID3D12CommandAllocator>  commandAllocator;

	if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
	{
		commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
		m_CommandAllocatorQueue.pop();

		Helpers::ThrowIfFailed(commandAllocator->Reset());
	}
	else
	{

		commandAllocator = CreateCommandAllocator();
		commandAllocator->SetName(L"CommandListAllocator");
	}

	if (!m_CommandListQueue.empty())
	{
		commandList = m_CommandListQueue.front();
		m_CommandListQueue.pop();

		Helpers::ThrowIfFailed(commandList->Reset(commandAllocator.Get(),nullptr));
	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
		commandList->SetName(L"CommandList");
	}
	Helpers::ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator),commandAllocator.Get()));

	return commandList;
}

ComPtr<ID3D12CommandQueue> GPUCommandQueue::GetCommandQueue()
{
	return m_CommandQueue;
}

uint64_t GPUCommandQueue::ExecuteCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	Helpers::ThrowIfFailed(commandList->Close());

	ID3D12CommandAllocator* commandAllocator;
	UINT dataSize = sizeof(commandAllocator);
	Helpers::ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator),&dataSize,&commandAllocator));

	m_CommandQueue->ExecuteCommandLists(1,CommandListCast(commandList.GetAddressOf()));
	uint64_t fenceValue = Signal();

	m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
	m_CommandListQueue.push(commandList);
	commandAllocator->Release();

	return fenceValue;
}

void GPUCommandQueue::Flush()
{
	uint64_t fenceValueForSignal = Signal();

	WaitForFenceValue(fenceValueForSignal);
}