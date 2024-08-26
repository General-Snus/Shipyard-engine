#include "ScriptGraphPin.h"
#include "ScriptGraphEdge.h"
#include "ScriptGraphNode.h"

ScriptGraphPin ScriptGraphPin::CreatePin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType,
                                         PinDirection aDirection, bool aHideLabelOnNode)
{
	ScriptGraphPin pin(aOwner, aLabel, aDirection, aType);
	pin.myLabelVisible = !aHideLabelOnNode;
	return pin;
}

ScriptGraphPin ScriptGraphPin::CreateDataPin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType,
	PinDirection aDirection, std::type_index aDataType, bool aHideLabelOnNode)
{
	ScriptGraphPin pin(aOwner, aLabel, aDirection, aType, aDataType);
	pin.myLabelVisible = !aHideLabelOnNode;
	return pin;
}

ScriptGraphPin ScriptGraphPin::CreateDynamicPin(ScriptGraphNode* aOwner, std::string_view aLabel,
	ScriptGraphPinType aType, PinDirection aDirection)
{
	if((!aOwner->HasFlag(GraphNodeFlag_DynamicInputPins) && aDirection == PinDirection::Input)
		|| (!aOwner->HasFlag(GraphNodeFlag_DynamicOutputPins) && aDirection == PinDirection::Output)
		)
	{
		throw std::runtime_error("Node is not configured to have dynamic pins!");
	}

	ScriptGraphPin pin(aOwner, aLabel, aDirection, aType);
	pin.myLabelVisible = true;
	pin.myIsDynamicPin = true;
	return pin;
}

ScriptGraphPin ScriptGraphPin::CreateDynamicDataPin(ScriptGraphNode* aOwner, std::string_view aLabel,
                                                    ScriptGraphPinType aType, PinDirection aDirection, std::type_index aDataType)
{
	if((!aOwner->HasFlag(GraphNodeFlag_DynamicInputPins) && aDirection == PinDirection::Input)
		|| (!aOwner->HasFlag(GraphNodeFlag_DynamicOutputPins) && aDirection == PinDirection::Output)
		)
	{
		throw std::runtime_error("Node is not configured to have dynamic pins!");
	}

	ScriptGraphPin pin(aOwner, aLabel, aDirection, aType, aDataType);
	pin.myLabelVisible = true;
	pin.myIsDynamicPin = true;
	return pin;
}

bool ScriptGraphPin::TryChangeDataType(const RegisteredType* aType)
{
	if(*aType != RegisteredType::InvalidType)
	{
		myData = TypedDataContainer::Create(aType);
		return true;
	}

	return false;
}

ScriptGraphPin::ScriptGraphPin(ScriptGraphNode* aOwner, std::string_view aLabel, PinDirection aDirection,
                               ScriptGraphPinType aType, std::type_index aDataType): NodeGraphPin(aOwner, aLabel, aDirection), myPinType(aType)
{
	// If we get a nullptr type we defer type data construction till later.
	if(aDataType != typeid(std::nullptr_t))
	{
		myData = TypedDataContainer::Create(aDataType);	
	}	
}
