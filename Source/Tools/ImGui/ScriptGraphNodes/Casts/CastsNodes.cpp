#include "pch.h"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

#include "CastsNodes.h"
#include <string>

IMPLEMENT_GRAPH_NODE(SGNode_FloatToString, ScriptGraphNode)
IMPLEMENT_GRAPH_NODE(SGNode_UUIDToString, ScriptGraphNode)
IMPLEMENT_GRAPH_NODE(SGNode_GameObjectToString, ScriptGraphNode)

SGNode_FloatToString::SGNode_FloatToString()
{
    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

NodeResult SGNode_FloatToString::DoOperation()
{
    float val = 0;
    GetPinData("Value", val);
    SetPinData("Text", std::to_string(val));
    return NoExec();
}

SGNode_UUIDToString::SGNode_UUIDToString()
{
    CreateDataPin<SY::UUID>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

NodeResult SGNode_UUIDToString::DoOperation()
{
    SY::UUID val = 0;
    GetPinData("Value", val);
    SetPinData("Text", std::to_string(val));
    return NoExec();
}

SGNode_GameObjectToString::SGNode_GameObjectToString()
{
    CreateDataPin<GameObject>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

NodeResult SGNode_GameObjectToString::DoOperation()
{
    GameObject val;
    GetPinData("Value", val);
    SetPinData("Text", val.GetName());
    return NoExec();
}
