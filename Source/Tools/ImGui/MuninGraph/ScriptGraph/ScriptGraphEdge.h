#pragma once
#include "ScriptGraphCommon.h"
#include "NodeGraph/NodeGraphEdge.h"

struct ScriptGraphEdge : public NodeGraphEdge
{
	ScriptGraphPinType Type;
};
