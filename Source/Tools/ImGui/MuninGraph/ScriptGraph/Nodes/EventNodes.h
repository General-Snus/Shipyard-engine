#pragma once
#include <Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphCommon.h>
#include <Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h>

class ScriptGraphEventNode : public ScriptGraphNode
{
  public:
    ScriptGraphEventNode();

    NodeResult DoOperation() override;

    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Events";
    }
    FORCEINLINE ScriptGraphNodeType GetNodeType() const override
    {
        return ScriptGraphNodeType::Event;
    }

    /**
     * \brief Returns the Entry Handle to use for this node, i.e. what to give to ScriptGraph::Execute.
     *		  By default it returns the base name of the node. For SGNode_MyEvent this is "MyEvent".
     */
    virtual FORCEINLINE std::string_view GetEntryHandle() const
    {
        return "";
    }
};
