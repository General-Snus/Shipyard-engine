#include "pch.h"
#include "SGNode_LogText.h"

#include <iostream>

IMPLEMENT_GRAPH_NODE(SGNode_LogText, ScriptGraphNode)

SGNode_LogText::SGNode_LogText()
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateExecPin("Out", PinDirection::Output, false);

	CreateDataPin<std::string>("Text", PinDirection::Input);
}

NodeResult SGNode_LogText::DoOperation()
{
	std::string text;
	if(GetPinData("Text", text))
	{
		std::cout << text << std::endl;
		return ExecPin("Out");
	}

	return Error("Could not read the Text message!");
}
