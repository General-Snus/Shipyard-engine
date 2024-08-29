#include "pch.h"

#include "SGNode_Tick.h"
#include <Engine/AssetManager/ComponentSystem/UUID.h>

IMPLEMENT_GRAPH_NODE(SGNode_Tick, ScriptGraphEventNode);
IMPLEMENT_GRAPH_NODE(SGNode_EventOnEnter, ScriptGraphEventNode);
IMPLEMENT_GRAPH_NODE(SGNode_EventOnExit, ScriptGraphEventNode);

SGNode_Tick::SGNode_Tick()
{
    CreateDataPin<float>("Delta Time", PinDirection::Output);
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
}

SGNode_EventOnEnter::SGNode_EventOnEnter()
{
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
    CreateDataPin<SY::UUID>("CollidedWith", PinDirection::Output);
}

SGNode_EventOnExit::SGNode_EventOnExit()
{
    CreateDataPin<SY::UUID>("UserID", PinDirection::Output);
    CreateDataPin<SY::UUID>("CollidedWith", PinDirection::Output);
}
