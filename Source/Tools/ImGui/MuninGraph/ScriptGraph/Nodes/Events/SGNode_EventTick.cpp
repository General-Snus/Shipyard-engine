#include "MuninGraph.pch.h"

#include "SGNode_EventTick.h"

void SGNode_EventTick::Init()
{
    CreateExecPin("Out", PinDirection::Output, true);
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
    CreateDataPin<float>("Delta Time", PinDirection::Output);
}

void SGNode_EventOnEnter::Init()
{
    CreateExecPin("Out", PinDirection::Output, true);
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
    CreateDataPin<SY::UUID>("CollidedWith", PinDirection::Output);
}

void SGNode_EventOnExit::Init()
{
    CreateExecPin("Out", PinDirection::Output, true);
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
    CreateDataPin<SY::UUID>("CollidedWith", PinDirection::Output);
}
