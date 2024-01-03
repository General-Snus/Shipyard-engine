#pragma once

#include "../../ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_MathAdd)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Add"; }
	std::string GetDescription() const override { return "Calculates the addition between two float values."; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};