#pragma once
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathAdd, ScriptGraphNode)
{
  public:
    SGNode_MathAdd();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Add";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(MVNode_MakeVector, ScriptGraphNode)
{
  public:
    MVNode_MakeVector();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Make Vector";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Linear Algebra";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(MVNode_BreakVector, ScriptGraphNode)
{
  public:
    MVNode_BreakVector();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Break Vector";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Linear Algebra";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(MVNode_Branch, ScriptGraphNode)
{
  public:
    MVNode_Branch();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Branch";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Operation";
    }
};

DECLARE_GRAPH_NODE(MVNode_ForLoop, ScriptGraphNode)
{
  public:
    MVNode_ForLoop();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "For";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Operation";
    }
};
DECLARE_GRAPH_NODE(MVNode_GetGameObject, ScriptGraphNode)
{
  public:
    MVNode_GetGameObject();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Get GameObject";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "ComponentSystem";
    }
};

DECLARE_GRAPH_NODE(MVNode_GetTransformPosition, ScriptGraphNode)
{
  public:
    MVNode_GetTransformPosition();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Get Transform Position";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Transform";
    }
};

DECLARE_GRAPH_NODE(MVNode_SetTransformPosition, ScriptGraphNode)
{
  public:
    MVNode_SetTransformPosition();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Set Transform Position";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Transform";
    }
};

DECLARE_GRAPH_NODE(MVNode_SetTransformScale, ScriptGraphNode)
{
  public:
    MVNode_SetTransformScale();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Set Transform Scale";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Transform";
    }
};

DECLARE_GRAPH_NODE(MVNode_SetTransformRotation, ScriptGraphNode)
{
  public:
    MVNode_SetTransformRotation();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Set Transform Rotation";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Transform";
    }
};

DECLARE_GRAPH_NODE(MVNode_GetCursorPosition, ScriptGraphNode)
{
  public:
    MVNode_GetCursorPosition();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Get Mouse cursor position";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Input";
    }
};

DECLARE_GRAPH_NODE(MVNode_Distance, ScriptGraphNode)
{
  public:
    MVNode_Distance();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Distance of a vector";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Linear Algebra";
    }
};

DECLARE_GRAPH_NODE(MVNode_VectorMinus, ScriptGraphNode)
{
  public:
    MVNode_VectorMinus();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Difference between two vectors";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Linear Algebra";
    }
};

DECLARE_GRAPH_NODE(MVNode_ScreenSpacePosition, ScriptGraphNode)
{
  public:
    MVNode_ScreenSpacePosition();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Distance between two vectors";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Linear Algebra";
    }
};
