#include "pch.h"

#include "SGNode_Math.h"
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/PersistentSystems/Scene.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/LinearAlgebra/Vector3.hpp>

IMPLEMENT_GRAPH_NODE(SGNode_MathAdd, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathSubtract, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathMultiply, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathDivide, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathPower, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathLogarithm, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathAbsolute, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathMinimum, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathMaximum, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathClamp, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathLessThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathGreaterThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathEqual, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathNotEqual, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathSign, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_MathModulus, ScriptGraphNode);

IMPLEMENT_GRAPH_NODE(MVNode_MakeVector, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_BreakVector, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_ScreenSpacePosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_VectorMinus, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_Distance, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetCursorPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetTransformRotation, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformRotation, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetTransformScale, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformScale, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetTransformPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetGameObject, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_IsGameObject, ScriptGraphNode);

SGNode_MathAdd::SGNode_MathAdd()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathAdd::DoOperation()
{
    float inA = 0;
    float inB = 0;

    if (GetPinData("A", inA) && GetPinData("B", inB))
    {
        const float result = inA + inB;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Something went wrong adding up A and B!");
}

SGNode_MathSubtract::SGNode_MathSubtract()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathSubtract::DoOperation()
{
    float inA = 0;
    float inB = 0;

    if (GetPinData("A", inA) && GetPinData("B", inB))
    {
        const float result = inA - inB;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Something went wrong subtracting B from A!");
}

SGNode_MathMultiply::SGNode_MathMultiply()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathMultiply::DoOperation()
{
    float inA = 0;
    float inB = 0;

    if (GetPinData("A", inA) && GetPinData("B", inB))
    {
        const float result = inA * inB;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Something went wrong multiplying A and B!");
}

SGNode_MathDivide::SGNode_MathDivide()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathDivide::DoOperation()
{
    float inA = 0;
    float inB = 1; // Initialize inB to avoid division by zero

    if (GetPinData("A", inA) && GetPinData("B", inB))
    {
        if (std::fabs(inB) > 1e-6) // Avoid division by zero with a tolerance
        {
            const float result = inA / inB;
            SetPinData("Result", result);
            return ExecPin("Out");
        }
        return Error("Division by zero error.");
    }

    return Error("Failed to get input data for division.");
}

SGNode_MathPower::SGNode_MathPower()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("Base", PinDirection::Input);
    CreateDataPin<float>("Exponent", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathPower::DoOperation()
{
    float base = 0;
    float exponent = 0;

    if (GetPinData("Base", base) && GetPinData("Exponent", exponent))
    {
        const float result = std::powf(base, exponent);
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for power operation.");
}

SGNode_MathLogarithm::SGNode_MathLogarithm()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<float>("Base", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathLogarithm::DoOperation()
{
    float value = 0;
    float base = 0;

    if (GetPinData("Value", value) && GetPinData("Base", base))
    {
        if (std::fabs(base) > 1e-6 && value > 0) // Avoid log of non-positive values and base zero
        {
            const float result = std::log2f(value) / std::log2f(base);
            SetPinData("Result", result);
            return ExecPin("Out");
        }
        return Error("Invalid value or base for logarithm operation.");
    }

    return Error("Failed to get data for logarithm operation.");
}

SGNode_MathAbsolute::SGNode_MathAbsolute()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathAbsolute::DoOperation()
{
    float value = 0;

    if (GetPinData("Value", value))
    {
        const float result = std::fabsf(value);
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for absolute value operation.");
}

SGNode_MathMinimum::SGNode_MathMinimum()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathMinimum::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const float result = std::min(a, b);
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for minimum operation.");
}

SGNode_MathMaximum::SGNode_MathMaximum()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathMaximum::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const float result = std::max(a, b);
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for maximum operation.");
}

SGNode_MathClamp::SGNode_MathClamp()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<float>("UpperBound", PinDirection::Input);
    CreateDataPin<float>("LowerBound", PinDirection::Input);
    CreateDataPin<float>("ClampedValue", PinDirection::Output);
}

NodeResult SGNode_MathClamp::DoOperation()
{
    float value = 0;
    float upperBound = 0;
    float lowerBound = 0;

    if (GetPinData("Value", value) && GetPinData("UpperBound", upperBound) && GetPinData("LowerBound", lowerBound))
    {
        const float clampedValue = std::clamp(value, lowerBound, upperBound);
        SetPinData("ClampedValue", clampedValue);
        return ExecPin("Out");
    }

    return Error("Failed to get data for clamping operation.");
}

SGNode_MathLessThan::SGNode_MathLessThan()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<bool>("Result", PinDirection::Output);
}

NodeResult SGNode_MathLessThan::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const bool result = a < b;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for less than operation.");
}

SGNode_MathGreaterThan::SGNode_MathGreaterThan()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<bool>("Result", PinDirection::Output);
}

NodeResult SGNode_MathGreaterThan::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const bool result = a > b;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for greater than operation.");
}

SGNode_MathEqual::SGNode_MathEqual()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<bool>("Result", PinDirection::Output);
}

NodeResult SGNode_MathEqual::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const bool result = a == b;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for equality check.");
}

SGNode_MathNotEqual::SGNode_MathNotEqual()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("B", PinDirection::Input);
    CreateDataPin<bool>("Result", PinDirection::Output);
}

NodeResult SGNode_MathNotEqual::DoOperation()
{
    float a = 0;
    float b = 0;

    if (GetPinData("A", a) && GetPinData("B", b))
    {
        const bool result = a != b;
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data for not equal operation.");
}

SGNode_MathSign::SGNode_MathSign()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Out", PinDirection::Output, true);
    CreateDataPin<float>("A", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathSign::DoOperation()
{
    float inA = 0;
    if (GetPinData("A", inA))
    {
        const float result = std::signbit(inA) ? -1.f : 1.f;
        SetPinData("Result", result);
        return ExecPin("Out");
    }
    return Error("Something went wrong");
}

SGNode_MathModulus::SGNode_MathModulus()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Out", PinDirection::Output, true);
    CreateDataPin<float>("Value", PinDirection::Input);
    CreateDataPin<float>("Remainder", PinDirection::Input);
    CreateDataPin<float>("Result", PinDirection::Output);
}

NodeResult SGNode_MathModulus::DoOperation()
{
    float inA = 0;
    float inB = 0;
    if (GetPinData("Value", inA) && GetPinData("Remainder", inB))
    {
        const float result = Mod<float>(inA, inB);
        SetPinData("Result", result);
        return ExecPin("Out");
    }
    return Error("Something went wrong");
}

MVNode_MakeVector::MVNode_MakeVector()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<float>("X", PinDirection::Input);
    CreateDataPin<float>("Y", PinDirection::Input);
    CreateDataPin<float>("Z", PinDirection::Input);
    CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

NodeResult MVNode_MakeVector::DoOperation()
{
    float X = 0, Y = 0, Z = 0;
    if (GetPinData("X", X) && GetPinData("Y", Y) && GetPinData("Z", Z))
    {
        Vector3f result(X, Y, Z);
        SetPinData("Result", result);
        return ExecPin("Out");
    }

    return Error("Failed to get data");
}

MVNode_BreakVector::MVNode_BreakVector()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("Vector", PinDirection::Input);
    CreateDataPin<float>("X", PinDirection::Output);
    CreateDataPin<float>("Y", PinDirection::Output);
    CreateDataPin<float>("Z", PinDirection::Output);
}

NodeResult MVNode_BreakVector::DoOperation()
{
    Vector3f vector;
    if (GetPinData("Vector", vector))
    {
        SetPinData("X", vector.x);
        SetPinData("Y", vector.y);
        SetPinData("Z", vector.z);
        return ExecPin("Out");
    }

    return Error("Failed to get data");
}

MVNode_GetGameObject::MVNode_GetGameObject()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<SY::UUID>("ID", PinDirection::Input);
    CreateDataPin<GameObject>("GameObject", PinDirection::Output);
}

NodeResult MVNode_GetGameObject::DoOperation()
{
    SY::UUID ID;
    if (GetPinData("ID", ID))
    {
        auto arg = Scene::ActiveManager().GetGameObject(ID);
        if (arg.IsValid())
        {
            SetPinData("GameObject", arg);
            return ExecPin("Out");
        }
        return Error("Could not find gameObject component");
    }
    return Error("Failed to get data");
}

MVNode_IsGameObject::MVNode_IsGameObject()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<std::string>("Name", PinDirection::Input);
    CreateDataPin<bool>("IsSame", PinDirection::Output);
}

NodeResult MVNode_IsGameObject::DoOperation()
{
    GameObject obj;
    std::string Name;
    if (GetPinData("GameObject", obj) && GetPinData("Name", Name))
    {
        if (obj.IsValid())
        {
            SetPinData("IsSame", (obj.GetName() == Name));
            return ExecPin("Out");
        }

        SetPinData("IsSame", false);
        return ExecPin("Out");
    }
    return Error("Failed to get data");
}

MVNode_GetTransformPosition::MVNode_GetTransformPosition()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("OutPosition", PinDirection::Output);
}

NodeResult MVNode_GetTransformPosition::DoOperation()
{
    GameObject object;
    Vector3f position;
    if (GetPinData("GameObject", object) && object.IsValid())
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            SetPinData("OutPosition", arg->GetPosition());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }
    return Error("Failed to get data");
}

MVNode_SetTransformPosition::MVNode_SetTransformPosition()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Position", PinDirection::Input);
    CreateDataPin<Vector3f>("OutPosition", PinDirection::Output);
}

NodeResult MVNode_SetTransformPosition::DoOperation()
{
    GameObject object;
    Vector3f position;
    if (GetPinData("GameObject", object) && GetPinData("Position", position))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetPosition(position);
            SetPinData("OutPosition", arg->GetPosition());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }

    return Error("Failed to get data");
}

MVNode_GetTransformScale::MVNode_GetTransformScale()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("OutScale", PinDirection::Output);
}

NodeResult MVNode_GetTransformScale::DoOperation()
{
    GameObject object;
    if (GetPinData("GameObject", object))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            SetPinData("OutScale", arg->GetScale());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }
    return Error("Failed to get data");
}

MVNode_SetTransformScale::MVNode_SetTransformScale()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Scale", PinDirection::Input);
    CreateDataPin<Vector3f>("New scale", PinDirection::Output);
}

NodeResult MVNode_SetTransformScale::DoOperation()
{
    GameObject object;
    Vector3f scale;
    if (GetPinData("GameObject", object) && GetPinData("Scale", scale))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetScale(scale);
            SetPinData("New scale", arg->GetScale());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }

    return Error("Failed to get data");
}

MVNode_GetTransformRotation::MVNode_GetTransformRotation()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("OutRotation", PinDirection::Output);
}

NodeResult MVNode_GetTransformRotation::DoOperation()
{
    GameObject object;
    if (GetPinData("GameObject", object))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            SetPinData("OutRotation", arg->GetRotation());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }
    return Error("Failed to get data");
}

MVNode_SetTransformRotation::MVNode_SetTransformRotation()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Rotation", PinDirection::Input);
    CreateDataPin<Vector3f>("New Rotation", PinDirection::Output);
}

NodeResult MVNode_SetTransformRotation::DoOperation()
{
    GameObject object;
    Vector3f rotation;
    if (GetPinData("GameObject", object) && GetPinData("Rotation", rotation))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetRotation(rotation);
            SetPinData("New Rotation", arg->GetPosition());
            return ExecPin("Out");
        }
        return Error("Could not find transform component");
    }
    return Error("Failed to get data");
}

MVNode_GetCursorPosition::MVNode_GetCursorPosition()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("CursorPosition", PinDirection::Output);
}

NodeResult MVNode_GetCursorPosition::DoOperation()
{
    const auto position = Input::GetMousePosition();
    SetPinData("CursorPosition", Vector3f((float)position.x, (float)position.y, 0));
    return ExecPin("Out");
}

MVNode_Distance::MVNode_Distance()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("Vector", PinDirection::Input);
    CreateDataPin<float>("Distance", PinDirection::Output);
}

NodeResult MVNode_Distance::DoOperation()
{
    Vector3f vector;
    if (GetPinData("Vector", vector))
    {
        SetPinData("Distance", vector.Length());
        return ExecPin("Out");
    }
    return Error("Could not find Vector");
}

MVNode_VectorMinus::MVNode_VectorMinus()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("VectorA", PinDirection::Input);
    CreateDataPin<Vector3f>("VectorB", PinDirection::Input);
    CreateDataPin<Vector3f>("Difference", PinDirection::Output);
}

NodeResult MVNode_VectorMinus::DoOperation()
{
    Vector3f vectorA;
    Vector3f vectorB;
    if (GetPinData("VectorA", vectorA) && GetPinData("VectorB", vectorB))
    {
        SetPinData("Difference", vectorA - vectorB);
        return ExecPin("Out");
    }
    return Error("Could not find Vector");
}

MVNode_ScreenSpacePosition::MVNode_ScreenSpacePosition()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("World position", PinDirection::Input);
    CreateDataPin<Vector3f>("Screen position", PinDirection::Output);
}

NodeResult MVNode_ScreenSpacePosition::DoOperation()
{
    Vector3f vector;
    if (GetPinData("World position", vector))
    {
        auto &camera = Scene::ActiveManager().GetCamera().GetComponent<cCamera>();
        Vector4f screenspacePos = camera.WoldSpaceToPostProjectionSpace(vector);
        SetPinData("Screen position", Vector3f(screenspacePos.x, screenspacePos.y, screenspacePos.z));
        return ExecPin("Out");
    }
    return Error("Could not find Vector");
}
