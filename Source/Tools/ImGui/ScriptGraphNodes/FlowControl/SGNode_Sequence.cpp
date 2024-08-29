#include "pch.h"

#include "SGNode_Sequence.h"
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphPin.h"
#include <Tools/Reflection/magic_enum/magic_enum.hpp>
#include <Tools/Utilities/Input/Input.hpp>

IMPLEMENT_GRAPH_NODE(SGNode_Sequence, ScriptGraphNode);

SGNode_Sequence::SGNode_Sequence()
{
    AddFlag(GraphNodeFlag_DynamicOutputPins);
    AddFlag(GraphNodeFlag_EditableOutputPins);

    CreateExecPin("In", PinDirection::Input);
    CreateExecPin("Then 0", PinDirection::Output);
}

NodeResult SGNode_Sequence::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
    ClearError();
    if (aEntryType == NodeEntryType::New)
    {
        myIndex = 0;
        RefreshExitPins();
    }

    if (myIndex < myExitPins.size())
    {
        const ScriptGraphPin &exitPin = GetPin(myExitPins[myIndex]);
        ExecPin(exitPin.GetLabel(),
                myIndex == myExitPins.size() - 1 ? NodeResultState::Finished : NodeResultState::InProgress);
    }

    return Error("Sequence ran out of Pins! This should not happen.");
}

#ifndef NOEDITOR
void SGNode_Sequence::OnUserAddedPin(ScriptGraphSchema *aSchema, size_t aPinId)
{
    RefreshExitPins();
    for (size_t i = 0; i < myExitPins.size(); i++)
    {
        RenamePin(myExitPins[i], "Temp " + std::to_string(i));
    }

    for (size_t i = 0; i < myExitPins.size(); i++)
    {
        RenamePin(myExitPins[i], "Then " + std::to_string(i));
    }
}

void SGNode_Sequence::OnUserRemovedPin(ScriptGraphSchema *aSchema, size_t aPinId)
{
    RefreshExitPins();
    for (size_t i = 0; i < myExitPins.size(); i++)
    {
        RenamePin(myExitPins[i], "Temp " + std::to_string(i));
    }

    for (size_t i = 0; i < myExitPins.size(); i++)
    {
        RenamePin(myExitPins[i], "Then " + std::to_string(i));
    }
}

void SGNode_Sequence::RefreshExitPins()
{
    myExitPins.clear();
    myExitPins.reserve(GetPins().size());
    for (const auto &[pinId, pin] : GetPins())
    {
        if (pin.GetPinDirection() == PinDirection::Output)
        {
            myExitPins.emplace_back(pinId);
        }
    }

    std::ranges::sort(myExitPins, [](const size_t &A, const size_t &B) { return A < B; });
}
#endif

IMPLEMENT_GRAPH_NODE(MVNode_Branch, ScriptGraphNode);

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

IMPLEMENT_GRAPH_NODE(MVNode_IsKeyPressed, ScriptGraphNode);

MVNode_IsKeyPressed::MVNode_IsKeyPressed()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Out", PinDirection::Output);

    CreateDataPin<std::string>("Key", PinDirection::Input);
    CreateDataPin<bool>("Condition", PinDirection::Output);
}

NodeResult MVNode_IsKeyPressed::DoOperation()
{
    std::string key;

    if (GetPinData("Key", key))
    {
        auto eKey = magic_enum::enum_cast<Keys>(key, magic_enum::case_insensitive);

        if (!eKey.has_value())
        {

            return Error("Invalid Key");
        }

        if (Input::IsKeyPressed(eKey.value()))
        {
            SetPinData("Condition", true);
            return ExecPin("Out");
        }
        else
        {
            SetPinData("Condition", false);
            return ExecPin("Out");
        }
    }
    return Error("No Key");
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

IMPLEMENT_GRAPH_NODE(MVNode_ForLoop, ScriptGraphNode);
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

IMPLEMENT_GRAPH_NODE(MVNode_Timer, ScriptGraphNode);
MVNode_Timer::MVNode_Timer()
{
    CreateExecPin("In", PinDirection::Input, true);

    CreateDataPin<int>("Wait:", PinDirection::Input);
    CreateDataPin<float>("Percentage", PinDirection::Output);
    CreateExecPin("Completed", PinDirection::Output);
    CreateExecPin("Waiting", PinDirection::Output);
}

NodeResult MVNode_Timer::DoOperation()
{
    static bool firstTime = true;
    if (firstTime)
    {
        auto startTime = std::chrono::system_clock::now();
        GetPinData("Wait:", sleepTimer);
        endTime = startTime + std::chrono::milliseconds(sleepTimer);
        firstTime = false;
    }

    const auto now = std::chrono::system_clock::now();
    if (now < endTime)
    {
        const auto diff = (endTime - now);
        const float perc =
            (float)std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / (float)sleepTimer;

        SetPinData("Percentage", perc);
        return ExecPin("Waiting");
    }

    SetPinData("Percentage", 1.0f);
    return ExecPin("Completed");
}
