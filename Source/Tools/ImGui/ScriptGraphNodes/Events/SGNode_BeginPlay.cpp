#include "pch.h"
#include "SGNode_BeginPlay.h"

IMPLEMENT_GRAPH_NODE(SGNode_BeginPlay, ScriptGraphEventNode);

SGNode_BeginPlay::SGNode_BeginPlay()
{
	// Event Nodes already have an Exec Pin called "Out" which gets called automatically.
	// You can add more pins here if you wish but it can only ever have ONE Exec Pin and
	// no Input Pins! Events are considered start nodes.
}