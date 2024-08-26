#pragma once

DECLARE_GRAPH_NODE(SGNode_FloatToString, ScriptGraphNode)
{
  public:
    SGNode_FloatToString();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Float to String";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Casts";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_UUIDToString, ScriptGraphNode)
{
  public:
    SGNode_UUIDToString();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "UUID to String";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Casts";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_GameObjectToString, ScriptGraphNode)
{
  public:
    SGNode_GameObjectToString();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "GameObject to String";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Casts";
    }
    NodeResult DoOperation() override;
};
