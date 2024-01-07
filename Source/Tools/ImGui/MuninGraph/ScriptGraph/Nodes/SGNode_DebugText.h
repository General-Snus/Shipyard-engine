#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_DebugText)
{
public:

	void Init() override;
	size_t DoOperation() override;
	std::string GetNodeTitle() const override { return "Debug Print"; }
	std::string GetDescription() const override { return "Prints a provided text string to stdout."; }
	std::string GetNodeCategory() const override { return "Debug"; }
	FORCEINLINE virtual bool IsDebugOnly() const override { return true; }
	GraphColor GetNodeHeaderColor() const override;
};
