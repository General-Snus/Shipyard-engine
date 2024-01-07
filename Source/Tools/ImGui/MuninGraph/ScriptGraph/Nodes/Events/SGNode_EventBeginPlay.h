#pragma once
#include "SGNode_EventBase.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphDerivedNode(SGNode_EventBeginPlay, SGNode_EventBase)
{
public:

	void Init() override;

	FORCEINLINE bool IsInternalOnly() const override { return true; }
	std::string GetNodeTitle() const override { return "Begin Play"; }
	std::string GetDescription() const override { return "An event node that fires when the object that owns it spawns."; }
};
