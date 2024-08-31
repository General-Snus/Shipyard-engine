#pragma once
#include "Tools/ImGui/MuninGraph/ScriptGraph/Nodes/EventNodes.h"
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_BeginPlay, ScriptGraphEventNode)
{
  public:
    SGNode_BeginPlay();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Begin Play";
    }

    // The handle we'll respond to when the graph looks for one in ScriptGraph::Execute.
    // If we don't override this then ScriptGraph will create its own based on the class name.
    // SGNode_BeginPlay will have a handle of "BeginPlay". This means our override here is
    // superfluous but it's here to show the functionality :).
    FORCEINLINE std::string_view GetEntryHandle() const override
    {
        return "BeginPlay";
    }
};
