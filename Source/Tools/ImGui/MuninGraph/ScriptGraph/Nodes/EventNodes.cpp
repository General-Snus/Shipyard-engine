#include "EventNodes.h"
#include "ScriptGraph/ScriptGraphEdge.h"

ScriptGraphEventNode::ScriptGraphEventNode()
{
	AddFlag(GraphNodeFlag_Unique);
	CreateExecPin("Out", PinDirection::Output, true);
}

NodeResult ScriptGraphEventNode::DoOperation()
{
	return ExecPin("Out", NodeResultState::Finished);
}
