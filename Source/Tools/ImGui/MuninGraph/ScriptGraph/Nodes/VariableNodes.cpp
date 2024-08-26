#include "VariableNodes.h"

#include "ScriptGraph/ScriptGraphEdge.h"
#include "ScriptGraph/ScriptGraphVariable.h"

ScriptGraphVariableNode::ScriptGraphVariableNode() = default;

ScriptGraphVariableNode::~ScriptGraphVariableNode() = default;

void ScriptGraphVariableNode::SetNodeVariable(ScriptGraphVariable *aVariable)
{
    myVariable = aVariable;

    for (const auto &[id, pin] : GetPins())
    {
        if (pin.GetPinType() != ScriptGraphPinType::Exec)
        {
            ChangePinDataType(id, myVariable->Data.GetType());

            if (pin.GetPinDirection() == PinDirection::Input)
            {
                mySetPinName = myVariable->Name;
                if (!pin.IsLabelVisible())
                    mySetPinName += "_IN";
                RenamePin(id, mySetPinName);
            }
            else
            {
                myGetPinName = myVariable->Name;
                if (!pin.IsLabelVisible())
                    myGetPinName += "_OUT";
                RenamePin(id, myGetPinName);
            }
        }
    }
}

  NodeResult ScriptGraphVariableNode::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
    if (!myVariable)
    {
        return Error("No valid variable set!");
    }

    return ScriptGraphNode::Enter(anEntryPinId, aEntryType);
}

IMPLEMENT_GRAPH_NODE_EX(SGNode_GetVariable, ScriptGraphVariableNode, CF_Internal);

  SGNode_GetVariable::SGNode_GetVariable()
{
    AddPin(ScriptGraphPin::CreateDataPin(this, "VAR-OUT", ScriptGraphPinType::Variable, PinDirection::Output,
                                         typeid(std::nullptr_t)));
}

  NodeResult SGNode_GetVariable::DoOperation()
{
    const ScriptGraphPin &outputPin = GetPin(myGetPinName);
    const TypedDataContainer &outputContainer = outputPin.GetDataContainer();

    std::vector<uint8_t> variableData;
    myVariable->Data.Serialize(variableData);
    outputContainer.Deserialize(variableData);

    return NoExec();
}

IMPLEMENT_GRAPH_NODE_EX(SGNode_SetVariable, ScriptGraphVariableNode, CF_Internal);

  NodeResult SGNode_SetVariable::DoOperation()
{
    const size_t inputPinId = GetPin(mySetPinName).GetUID();
    bool pinFound = false;
    const ScriptGraphPin &inputPin = GetDataSourcePin(inputPinId, pinFound);
    const TypedDataContainer &inputContainer = inputPin.GetDataContainer();

    std::vector<uint8_t> variableData;
    inputContainer.Serialize(variableData);
    myVariable->Data.Deserialize(variableData);

    const ScriptGraphPin &outputPin = GetPin(myGetPinName);
    const TypedDataContainer &outputContainer = outputPin.GetDataContainer();

    outputContainer.Deserialize(variableData);

    return ExecPin("Out");
}

  SGNode_SetVariable::SGNode_SetVariable()
{
    CreateExecPin("In", PinDirection::Input, true);
    CreateExecPin("Out", PinDirection::Output, true);

    AddPin(ScriptGraphPin::CreateDataPin(this, "VAR-IN", ScriptGraphPinType::Data, PinDirection::Input,
                                         typeid(std::nullptr_t), false));
    AddPin(ScriptGraphPin::CreateDataPin(this, "VAR-OUT", ScriptGraphPinType::Variable, PinDirection::Output,
                                         typeid(std::nullptr_t), true));
}
