#include "MuninGraph.pch.h"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

#include "SGNode_CastToString.h"

#include "ScriptGraph/Types/RegisterScriptGraphTypes.h"

void SGNode_FloatToString::Init()
{
    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_FloatToString::DoOperation()
{
    float val = 0;
    GetPinData("Value", val);
    SetPinData("Text", std::to_string(val));
    return Exit();
}

void SGNode_UUIDToString::Init()
{
    CreateDataPin<SY::UUID>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_UUIDToString::DoOperation()
{
    SY::UUID val = 0;
    GetPinData("Value", val);
    SetPinData("Text", std::to_string(val));
    return Exit();
}

void SGNode_GameObjectToString::Init()
{
    CreateDataPin<GameObject>("Value", PinDirection::Input);
    CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_GameObjectToString::DoOperation()
{
    GameObject val;
    GetPinData("Value", val);
    SetPinData("Text", val.GetName());
    return Exit();
}
