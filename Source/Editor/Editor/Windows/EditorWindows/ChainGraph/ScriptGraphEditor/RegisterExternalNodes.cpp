#include "Engine/PersistentSystems/PersistentSystems.pch.h"

#include "Engine/AssetManager/ComponentSystem/Components/CameraComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Engine/PersistentSystems/Scene.h"
#include "RegisterExternalNodes.h"
#include <Tools/Utilities/Input/EnumKeys.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
//
// void MVNode_TestNode::Init()
//{
//	CreateExecPin("In",PinDirection::Input,true);
//	CreateExecPin("Out",PinDirection::Output,true);
//	CreateDataPin<float>("Value",PinDirection::Input);
//}
//
// size_t MVNode_TestNode::DoOperation()
//{
//	return ExitViaPin("Out");
//}
//
// void MVNode_TestExecless::Init()
//{
//	CreateDataPin<bool>("Test",PinDirection::Input);
//	CreateDataPin<bool>("Value",PinDirection::Output);
//}
//
// size_t MVNode_TestExecless::DoOperation()
//{
//	bool b = false;
//	GetPinData("Test",b);
//	SetPinData("Value",b);
//	return Exit();
//}
//
// void MVNode_TestMultiInput::Init()
//{
//	CreateDataPin<float>("A",PinDirection::Input);
//	CreateDataPin<float>("B",PinDirection::Input);
//	CreateDataPin<float>("C",PinDirection::Input);
//	CreateDataPin<float>("Result",PinDirection::Output);
//}
//
// size_t MVNode_TestMultiInput::DoOperation()
//{
//	return Exit();
//}
//
// bool MVNode_TestMultiInput::IsSimpleNode() const
//{
//	return false;
//}

void MVNode_MakeVector::Init()
{
    CreateDataPin<float>("X", PinDirection::Input);
    CreateDataPin<float>("Y", PinDirection::Input);
    CreateDataPin<float>("Z", PinDirection::Input);
    CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t MVNode_MakeVector::DoOperation()
{
    float X = 0, Y = 0, Z = 0;
    if (GetPinData("X", X) && GetPinData("Y", Y) && GetPinData("Z", Z))
    {
        Vector3f result(X, Y, Z);
        SetPinData("Result", result);
        return Exit();
    }

    return 0;
}

void MVNode_BreakVector::Init()
{
    CreateDataPin<Vector3f>("Vector", PinDirection::Input);
    CreateDataPin<float>("X", PinDirection::Output);
    CreateDataPin<float>("Y", PinDirection::Output);
    CreateDataPin<float>("Z", PinDirection::Output);
}

size_t MVNode_BreakVector::DoOperation()
{
    Vector3f vector;
    if (GetPinData("Vector", vector))
    {
        SetPinData("X", vector.x);
        SetPinData("Y", vector.y);
        SetPinData("Z", vector.z);
        return Exit();
    }

    return 0;
}

void MVNode_Branch::Init()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("True", PinDirection::Output);
    CreateExecPin("False", PinDirection::Output);

    CreateDataPin<bool>("Condition", PinDirection::Input);
}

size_t MVNode_Branch::DoOperation()
{
    bool condition = false;
    if (GetPinData("Condition", condition))
    {
        return ExitViaPin(condition ? "True" : "False");
    }

    return 0;
}

void MVNode_ForLoop::Init()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Loop", PinDirection::Output);

    CreateDataPin<int>("Start", PinDirection::Input);
    CreateDataPin<int>("End", PinDirection::Input);
    CreateDataPin<int>("Index", PinDirection::Output);
    CreateExecPin("Completed", PinDirection::Output);
}

size_t MVNode_ForLoop::DoOperation()
{
    int start = 0;
    int end = 0;
    if (GetPinData("Start", start) && GetPinData("End", end))
    {
        for (int i = start; i < end; i++)
        {
            SetPinData("Index", i);
            ExitViaPin("Loop");
        }
        return ExitViaPin("Completed");
    }
    return 0;
}

void MVNode_GetGameObject::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<SY::UUID>("ID", PinDirection::Input);
    CreateDataPin<GameObject>("GameObject", PinDirection::Output);
}

size_t MVNode_GetGameObject::DoOperation()
{
    SY::UUID ID;
    if (GetPinData("ID", ID))
    {
        auto arg = Scene::ActiveManager().GetGameObject(ID);
        if (arg.IsValid())
        {
            SetPinData("GameObject", arg);
            return ExitViaPin("Out");
        }
        return ExitWithError("Could not find gameObject component");
    }
    return 0;
}

void MVNode_GetTransformPosition::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("OutPosition", PinDirection::Output);
}

size_t MVNode_GetTransformPosition::DoOperation()
{
    GameObject object;
    Vector3f position;
    if (GetPinData("GameObject", object) && object.IsValid())
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            SetPinData("OutPosition", arg->GetPosition());
            return ExitViaPin("Out");
        }
        return ExitWithError("Could not find transform component");
    }
    return 0;
}

void MVNode_SetTransformPosition::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Position", PinDirection::Input);
    CreateDataPin<Vector3f>("OutPosition", PinDirection::Output);
}

size_t MVNode_SetTransformPosition::DoOperation()
{
    GameObject object;
    Vector3f position;
    if (GetPinData("GameObject", object) && GetPinData("Position", position))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetPosition(position);
            SetPinData("OutPosition", arg->GetPosition());
            return ExitViaPin("Out");
        }
        return ExitWithError("Could not find transform component");
    }

    return 0;
}

void MVNode_SetTransformScale::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Scale", PinDirection::Input);
    CreateDataPin<Vector3f>("New scale", PinDirection::Output);
}

size_t MVNode_SetTransformScale::DoOperation()
{
    GameObject object;
    Vector3f scale;
    if (GetPinData("GameObject", object) && GetPinData("Scale", scale))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetScale(scale);
            SetPinData("New scale", arg->GetScale());
            return ExitViaPin("Out");
        }
        return ExitWithError("Could not find transform component");
    }

    return 0;
}

void MVNode_SetTransformRotation::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<GameObject>("GameObject", PinDirection::Input);
    CreateDataPin<Vector3f>("Rotation", PinDirection::Input);
    CreateDataPin<Vector3f>("New Rotation", PinDirection::Output);
}

size_t MVNode_SetTransformRotation::DoOperation()
{
    GameObject object;
    Vector3f rotation;
    if (GetPinData("GameObject", object) && GetPinData("Rotation", rotation))
    {
        if (auto *arg = object.TryGetComponent<Transform>())
        {
            arg->SetRotation(rotation);
            SetPinData("New Rotation", arg->GetPosition());
            return ExitViaPin("Out");
        }
        return ExitWithError("Could not find transform component");
    }
    return 0;
}

void MVNode_GetCursorPosition::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("CursorPosition", PinDirection::Output);
}

size_t MVNode_GetCursorPosition::DoOperation()
{
    const auto position = Input::GetMousePosition();
    SetPinData("CursorPosition", Vector3f((float)position.x, (float)position.y, 0));
    return ExitViaPin("Out");
}

void MVNode_Distance::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("Vector", PinDirection::Input);
    CreateDataPin<float>("Distance", PinDirection::Output);
}

size_t MVNode_Distance::DoOperation()
{
    Vector3f vector;
    if (GetPinData("Vector", vector))
    {
        SetPinData("Distance", vector.Length());
        return ExitViaPin("Out");
    }
    return ExitWithError("Could not find Vector");
}

void MVNode_VectorMinus::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("VectorA", PinDirection::Input);
    CreateDataPin<Vector3f>("VectorB", PinDirection::Input);
    CreateDataPin<Vector3f>("Difference", PinDirection::Output);
}

size_t MVNode_VectorMinus::DoOperation()
{
    Vector3f vectorA;
    Vector3f vectorB;
    if (GetPinData("VectorA", vectorA) && GetPinData("VectorB", vectorB))
    {
        SetPinData("Difference", vectorA - vectorB);
        return ExitViaPin("Out");
    }
    return ExitWithError("Could not find Vector");
}

void MVNode_ScreenSpacePosition::Init()
{
    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<Vector3f>("World position", PinDirection::Input);
    CreateDataPin<Vector3f>("Screen position", PinDirection::Output);
}

size_t MVNode_ScreenSpacePosition::DoOperation()
{
    Vector3f vector;
    if (GetPinData("World position", vector))
    {
        auto &camera = Scene::ActiveManager().GetCamera().GetComponent<cCamera>();
        Vector4f screenspacePos = camera.WoldSpaceToPostProjectionSpace(vector);
        SetPinData("Screen position", Vector3f(screenspacePos.x, screenspacePos.y, screenspacePos.z));
        return ExitViaPin("Out");
    }
    return ExitWithError("Could not find Vector");
}
