#include "pch.h"

#include "SGNode_Math.h"
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/PersistentSystems/Scene.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/LinearAlgebra/Vector3.hpp>

IMPLEMENT_GRAPH_NODE(SGNode_MathAdd, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_MakeVector, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_BreakVector, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_ScreenSpacePosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_VectorMinus, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_Distance, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetCursorPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformRotation, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformScale, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_SetTransformPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetTransformPosition, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_GetGameObject, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_ForLoop, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(MVNode_Branch, ScriptGraphNode);

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

MVNode_MakeVector::MVNode_MakeVector()
{
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

MVNode_Branch::MVNode_Branch()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("True", PinDirection::Output);
    CreateExecPin("False", PinDirection::Output);

    CreateDataPin<bool>("Condition", PinDirection::Input);
}

NodeResult MVNode_Branch::DoOperation()
{
    bool condition = false;
    if (GetPinData("Condition", condition))
    {
        return ExecPin(condition ? "True" : "False");
    }

    return Error("Failed to get data");
}

MVNode_ForLoop::MVNode_ForLoop()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Loop", PinDirection::Output);

    CreateDataPin<int>("Start", PinDirection::Input);
    CreateDataPin<int>("End", PinDirection::Input);
    CreateDataPin<int>("Index", PinDirection::Output);
    CreateExecPin("Completed", PinDirection::Output);
}

NodeResult MVNode_ForLoop::DoOperation()
{
    int start = 0;
    int end = 0;
    if (GetPinData("Start", start) && GetPinData("End", end))
    {
        for (int i = start; i < end; i++)
        {
            SetPinData("Index", i);
            ExecPin("Loop");
        }
        return ExecPin("Completed");
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
