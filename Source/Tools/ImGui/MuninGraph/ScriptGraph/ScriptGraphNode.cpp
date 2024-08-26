#include "ScriptGraphNode.h"

#include <ranges>

#include "ScriptGraph.h"
#include "ScriptGraphCommon.h"
#include "ScriptGraphEdge.h"
#include "ScriptGraphPayload.h"
#include "ScriptGraphPin.h"
#include "ScriptGraphSchema.h"

NodeResult ScriptGraphNode::Enter([[maybe_unused]] size_t anEntryPinId, [[maybe_unused]] NodeEntryType aEntryType)
{
	ClearError();
	return DoOperation();
}

const std::vector<std::type_index>& ScriptGraphNode::GetSupportedSchemas() const
{
	static std::vector<std::type_index> supportedSchemas =
	{
		typeid(ScriptGraphSchema)
	};

	return supportedSchemas;
}

void ScriptGraphNode::DeliverPayload(const ScriptGraphPayload& aPayload)
{
	for(auto& pin : GetPins() | std::views::values)
	{
		if (const auto dataIt = aPayload.myData.find(pin.GetLabel()); dataIt != aPayload.myData.end())
		{
			const TypedDataContainer& payloadData = dataIt->second;
			TypedDataContainer& pinData = GetMutablePin(pin.GetUID()).GetMutableDataContainer();
			if(payloadData.GetType() == pinData.GetType())
			{
				pinData = payloadData;
			}
		}
	}
}

void ScriptGraphNode::CreateExecPin(std::string_view aLabel, PinDirection aDirection, bool showLabelOnNode)
{
	AddPin(ScriptGraphPin::CreatePin(this, aLabel, ScriptGraphPinType::Exec, aDirection, !showLabelOnNode));
	myIsExecNode = true;
}

void ScriptGraphNode::CreateDataPin(std::string_view aLabel, PinDirection aDirection, std::type_index aType,
	bool hideLabelOnNode)
{
	AddPin(ScriptGraphPin::CreateDataPin(this, aLabel, ScriptGraphPinType::Data, aDirection, aType, hideLabelOnNode));
}

void ScriptGraphNode::RemovePin(size_t aPinID)
{
	const ScriptGraphPin& pin = GetPin(aPinID);
	if(pin.IsDynamicPin())
		NodeGraphNode::RemovePin(aPinID);

}

NodeResult ScriptGraphNode::NoExec()
{
	// We're done, we have nothing else to execute.
	return { NodeResultState::Finished, 0 };
}

NodeResult ScriptGraphNode::ExecPin(std::string_view aPinLabel, NodeResultState aState /*= NodeResultState::Finished*/)
{
	const ScriptGraphPin& pin = GetPin(aPinLabel);
	assert(pin.GetPinType() == ScriptGraphPinType::Exec && "Only Exec pins can be used in ExecPin!");

	return { aState, pin.GetUID() };
}

NodeResult ScriptGraphNode::Error(std::string_view aErrorMessage)
{
	// If something went wrong, it needs reporting.
	myHasError = true;
	myErrorMessage = aErrorMessage;

	return { NodeResultState::Error, 0 };
}

void ScriptGraphNode::ClearError()
{
	myHasError = false;
	myErrorMessage.clear();
}

bool ScriptGraphNode::ChangePinDataType(size_t aPinId, const RegisteredType* aType)
{
	ScriptGraphPin& pin = GetMutablePin(aPinId);
	return pin.TryChangeDataType(aType);
}

const ScriptGraphPin& ScriptGraphNode::GetDataSourcePin(size_t aPinId, bool& outFound) const
{
	outFound = false;

	const ScriptGraphPin& pin = GetPin(aPinId);
	const PinDirection pinDir = pin.GetPinDirection();
	if(pinDir == PinDirection::Output)
	{
		throw std::runtime_error("Cannot fetch Pin Data from Output pins!");
	}

	outFound = true;

	if(pin.IsConnected())
	{
		const ScriptGraphEdge& pinEdge = GetOwner()->GetEdgeFromId(pin.GetEdges()[0]);
		return GetOwner()->GetPinFromId(pinEdge.FromId);
	}

	return pin;
}

#ifndef NOEDITOR
void ScriptGraphNode::OnUserChangedPinValue([[maybe_unused]] ScriptGraphSchema* aSchema, [[maybe_unused]] size_t aPinId)
{  }

void ScriptGraphNode::OnUserAddedPin([[maybe_unused]] ScriptGraphSchema* aSchema, [[maybe_unused]] size_t aPinId)
{  }

void ScriptGraphNode::OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId)
{  }
#endif
