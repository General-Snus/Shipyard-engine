#include "MuninGraph.pch.h"
#include "SGNode_EventBase.h"

GraphColor SGNode_EventBase::GetNodeHeaderColor() const
{
	return GraphColor(255, 0, 0, 128);
}

size_t SGNode_EventBase::DoOperation()
{
	return ExitViaPin("Out");
}