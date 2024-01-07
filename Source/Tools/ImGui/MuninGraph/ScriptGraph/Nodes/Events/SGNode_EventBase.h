#pragma once
#include "../../ScriptGraphNode.h"

BeginScriptGraphBaseNode(SGNode_EventBase)
{
public:

	void Init() override {};

	[[nodiscard]] GraphColor GetNodeHeaderColor() const override;
	std::string GetNodeCategory() const override { return "Events"; };

	size_t DoOperation() override;

	FORCEINLINE bool IsEntryNode() const override { return true; }
	FORCEINLINE bool IsInternalOnly() const override { return false; }
	FORCEINLINE unsigned MaxInstancesPerGraph() const override { return 1; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::Event; }
};
