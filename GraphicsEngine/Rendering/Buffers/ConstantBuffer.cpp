#include "GraphicsEngine.pch.h"
#include "ConstantBuffer.h"

ConstantBufferBase::ConstantBufferBase(const void* aPtr, size_t aDataSize)
	: myDataSize(aDataSize), myDataPtr(aPtr)
{
}

bool ConstantBufferBase::Initialize()
{
	return RHI::CreateConstantBuffer(myBuffer, myDataSize);
}

ConstantBufferBase::~ConstantBufferBase()
{
	myBuffer.Reset();
}