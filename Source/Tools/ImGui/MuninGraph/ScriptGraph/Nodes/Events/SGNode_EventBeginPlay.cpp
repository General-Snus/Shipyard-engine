#include "MuninGraph.pch.h"
#include "SGNode_EventBeginPlay.h"

void SGNode_EventBeginPlay::Init()
{
	CreateExecPin("Out", PinDirection::Output, true);
}