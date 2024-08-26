#pragma once
#include <Tools/ImGui/MuninGraph/ScriptGraph/Nodes/EventNodes.h>
#include <Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h>

DECLARE_GRAPH_NODE(SGNode_Tick, ScriptGraphEventNode)
{
  public:
    SGNode_Tick();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Tick";
    }
};

DECLARE_GRAPH_NODE(SGNode_EventOnEnter, ScriptGraphEventNode)
{
  public:
    SGNode_EventOnEnter();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "OnEnter";
    }
};

DECLARE_GRAPH_NODE(SGNode_EventOnExit, ScriptGraphEventNode)
{
  public:
    SGNode_EventOnExit();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "OnExit";
    }
};
