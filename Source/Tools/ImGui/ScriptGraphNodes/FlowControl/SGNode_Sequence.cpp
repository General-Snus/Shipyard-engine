#include "pch.h"

#include "SGNode_Sequence.h"
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphPin.h"

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
