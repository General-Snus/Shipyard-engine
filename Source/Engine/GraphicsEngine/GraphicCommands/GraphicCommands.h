#pragma once

#include <Tools/ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <Tools/ThirdParty\CU\Math.hpp>

#include <Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/LineBuffer.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/G_buffer.h>

typedef CU::Matrix4x4<float>  Matrix;
 
class GraphicCommandBase
{
protected:
	FrameBuffer& GetFrameBuffer();
	ObjectBuffer& GetObjectBuffer();
	LineBuffer& GetLineBuffer();
	LightBuffer& GetLightBuffer();
	G_Buffer& GetGBuffer();
public:
	virtual ~GraphicCommandBase() = default;
	virtual void ExecuteAndDestroy() = 0;
	virtual void Destroy() = 0;
	GraphicCommandBase* Next;
};

class GraphicsCommandListIterator;
class GraphicsCommandList
{
	friend class GraphicsCommandListIterator;
public:
	GraphicsCommandList();
	~GraphicsCommandList();
	void Initialize(size_t aSize = 1*MegaByte); //5mb default

	template<typename CommandClass, typename ...Args>
	void AddCommand(Args ... arguments)
	{
		const size_t commandSize = sizeof(CommandClass);
		if (myCursor + commandSize > mySize)
		{
			throw std::out_of_range("CommandList is full");
		}
		GraphicCommandBase* ptr = reinterpret_cast<GraphicCommandBase*>(myData + myCursor);
		myCursor += commandSize;
		::new (ptr) CommandClass(arguments ...);
		*myLink = ptr;
		myLink = &ptr->Next;
	}

	void Execute();
	void StartOver();
	void Reset();

private:
	uint8_t* myData = nullptr;
	size_t myCursor = 0;
	size_t mySize = 0;

	GraphicCommandBase* myRoot = nullptr;
	GraphicCommandBase** myLink = nullptr;

	bool isExecuting = false;
	bool isFinished = false;
};

class GraphicsCommandListIterator
{
	GraphicCommandBase* myPtr = nullptr;
public:
	GraphicsCommandListIterator(const GraphicsCommandList& lst);
	GraphicCommandBase* Next();

	FORCEINLINE bool HasCommand() const 
	{
		return myPtr; 
	}
};   