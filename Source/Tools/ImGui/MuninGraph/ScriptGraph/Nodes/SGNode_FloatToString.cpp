#include "MuninGraph.pch.h"
#include "SGNode_FloatToString.h"

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
