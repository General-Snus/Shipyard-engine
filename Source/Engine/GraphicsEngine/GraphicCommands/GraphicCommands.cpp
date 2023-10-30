#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "GraphicCommands.h" 
#include <Tools/Utilities/Game/Timer.h>
#include <assert.h>
#include <Tools/ImGui/imgui.h>

FrameBuffer& GraphicCommandBase::GetFrameBuffer()
{
	return GraphicsEngine::Get().myFrameBuffer;
}
ObjectBuffer& GraphicCommandBase::GetObjectBuffer()
{
	return GraphicsEngine::Get().myObjectBuffer;
}
G_Buffer& GraphicCommandBase::GetGBuffer()
{
	return GraphicsEngine::Get().myG_Buffer;
}
LineBuffer& GraphicCommandBase::GetLineBuffer()
{
	return GraphicsEngine::Get().myLineBuffer;
}
LightBuffer& GraphicCommandBase::GetLightBuffer()
{
	return GraphicsEngine::Get().myLightBuffer;
}

GraphicsCommandList::GraphicsCommandList() = default;

GraphicsCommandList::~GraphicsCommandList()
{
	GraphicsCommandListIterator it(*this);
	while(it.HasCommand())
	{
		GraphicCommandBase* cmd = it.Next();
		cmd->Destroy();
	}
	delete[] myData;
	myData = nullptr;
	myRoot = nullptr;
	myLink = nullptr;
}

void GraphicsCommandList::Initialize(size_t aSize)
{
	mySize = aSize;
	myData = new uint8_t[mySize];
	memset(myData,0,mySize);

	myRoot = std::bit_cast<GraphicCommandBase*>(myData);
	myLink = &myRoot;
}

void GraphicsCommandList::Execute()
{
	if(!isExecuting && !isFinished)
	{
		isExecuting = true;
		GraphicsCommandListIterator it(*this);
		while(it.HasCommand())
		{
			GraphicCommandBase* cmd = it.Next();
			cmd->ExecuteAndDestroy(); 
		}
		isFinished = true;
		isExecuting = false;
	}
}

void GraphicsCommandList::ForceSetDone()
{
	isFinished = true;
}

void GraphicsCommandList::StartOver()
{
	myRoot = std::bit_cast<GraphicCommandBase*>(myData);
	myLink = &myRoot;
	myCursor = 0;
	isFinished = false;
}

void GraphicsCommandList::Reset()
{
	if(isFinished)
	{
		memset(myData,0,mySize);
		myRoot = std::bit_cast<GraphicCommandBase*>(myData);
		myLink = &myRoot;
		myCursor = 0;
		isFinished = false;
	}
}

GraphicsCommandListIterator::GraphicsCommandListIterator(const GraphicsCommandList& lst) : myPtr(lst.myRoot)
{}

GraphicCommandBase* GraphicsCommandListIterator::Next()
{
	GraphicCommandBase* cmd = myPtr;
	myPtr = cmd->Next;
	return cmd;
} 