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

DECLARE_GRAPH_NODE(SGNode_MathSubtract, ScriptGraphNode)
{
  public:
    SGNode_MathSubtract();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Subtract";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathMultiply, ScriptGraphNode)
{
  public:
    SGNode_MathMultiply();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Multiply";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathDivide, ScriptGraphNode)
{
  public:
    SGNode_MathDivide();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Divide";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathPower, ScriptGraphNode)
{
  public:
    SGNode_MathPower();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Power";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathLogarithm, ScriptGraphNode)
{
  public:
    SGNode_MathLogarithm();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Logarithm";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathAbsolute, ScriptGraphNode)
{
  public:
    SGNode_MathAbsolute();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Absolute";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathMinimum, ScriptGraphNode)
{
  public:
    SGNode_MathMinimum();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Min";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathMaximum, ScriptGraphNode)
{
  public:
    SGNode_MathMaximum();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Max";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathClamp, ScriptGraphNode)
{
  public:
    SGNode_MathClamp();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Clamp";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathLessThan, ScriptGraphNode)
{
  public:
    SGNode_MathLessThan();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Less Than";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathGreaterThan, ScriptGraphNode)
{
  public:
    SGNode_MathGreaterThan();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Greater Than";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathEqual, ScriptGraphNode)
{
  public:
    SGNode_MathEqual();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Equals";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathNotEqual, ScriptGraphNode)
{
  public:
    SGNode_MathNotEqual();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Not Equals";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathSign, ScriptGraphNode)
{
  public:
    SGNode_MathSign();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Sign";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Math";
    }
    NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_MathModulus, ScriptGraphNode)
{
  public:
    SGNode_MathModulus();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Modulus";
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

DECLARE_GRAPH_NODE(MVNode_IsGameObject, ScriptGraphNode)
{
  public:
    MVNode_IsGameObject();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "IsNameEquals";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "ComponentSystem";
    }
};

DECLARE_GRAPH_NODE(MVNode_IsGameObjectActive, ScriptGraphNode)
{
  public:
    MVNode_IsGameObjectActive();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "IsGameObjectActive";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "ComponentSystem";
    }
};

DECLARE_GRAPH_NODE(MVNode_SetGameObjectActive, ScriptGraphNode)
{
  public:
    MVNode_SetGameObjectActive();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return " SetGameObjectActive";
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

DECLARE_GRAPH_NODE(MVNode_GetTransformScale, ScriptGraphNode)
{
  public:
    MVNode_GetTransformScale();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Get Transform Scale";
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

DECLARE_GRAPH_NODE(MVNode_GetTransformRotation, ScriptGraphNode)
{
  public:
    MVNode_GetTransformRotation();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Get Transform Rotation";
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
