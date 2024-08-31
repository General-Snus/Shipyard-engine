#pragma once
#include <Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h>

class ScriptGraphVariableNode : public ScriptGraphNode
{
    friend class ScriptGraphSchema;

  public:
    ScriptGraphVariableNode();
    ~ScriptGraphVariableNode() override;

    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Variables";
    }
    FORCEINLINE ScriptGraphNodeType GetNodeType() const override
    {
        return ScriptGraphNodeType::Variable;
    }
    FORCEINLINE const ScriptGraphVariable *GetVariable() const
    {
        return myVariable;
    }

  protected:
    ScriptGraphVariable *myVariable;
    std::string myGetPinName;
    std::string mySetPinName;

    void SetNodeVariable(ScriptGraphVariable *aVariable);
    NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;
};

 DECLARE_GRAPH_NODE(SGNode_GetVariable, ScriptGraphVariableNode)
{
  public:
    SGNode_GetVariable();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "GET";
    }

    NodeResult DoOperation() override;
};

 DECLARE_GRAPH_NODE(SGNode_SetVariable, ScriptGraphVariableNode)
{
  public:
    SGNode_SetVariable();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "SET";
    }

    NodeResult DoOperation() override;
};
