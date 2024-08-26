#include "ScriptGraphSchema.h"

#include <ranges>

#include "MuninGraph.h"
#include "ScriptGraph.h"
#include "ScriptGraphNode.h"
#include "ScriptGraphEdge.h"
#include "ScriptGraphPin.h"
#include "NodeGraph/NodeGraphSchema.h"
#include "Nodes/EventNodes.h"
#include "Nodes/VariableNodes.h"

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::CreateNode(const RegisteredNodeClass& aClass)
{
	std::shared_ptr<ScriptGraphNode> newNode = std::static_pointer_cast<ScriptGraphNode>(aClass.New());

	if(newNode->GetNodeType() == ScriptGraphNodeType::Event)
	{
		size_t numInputPins = 0;
		size_t numOutputPins = 0;
		for(const auto& pin : newNode->myPins | std::views::values)
		{
			if(pin.GetPinDirection() == PinDirection::Output)
			{
				numOutputPins++;
			}
			else
			{
				numInputPins++;
			}
		}

		if(numInputPins > 0)
		{
			throw std::runtime_error("Event nodes cannot have Input Pins!");
		}
	}

	if(RegisterNode(newNode))
	{
		if(newNode->GetNodeType() == ScriptGraphNodeType::Event)
		{
			ScriptGraph* graph = GetMutableGraph();
			const auto& newNodeUID = AsObjectUIDSharedPtr(newNode);
			const std::shared_ptr<ScriptGraphEventNode>& newEventNode = std::static_pointer_cast<ScriptGraphEventNode>(newNode);

			// Check if the node has a handle.
			std::string entryHandle(newEventNode->GetEntryHandle());
			if(entryHandle.empty())
			{
				const size_t cutPos = aClass.Name.find_first_of('_');
				entryHandle = aClass.Name.substr(cutPos + 1);
			}

			if(entryHandle.empty())
			{
				throw std::runtime_error("Node Entry Handle is empty! Please override the appropriate function in the node or give it an appropriate name!");
			}

			graph->myEntryPoints.emplace(entryHandle, newNodeUID->GetUID());
			graph->myNodeIdToEntryPoint.emplace(newNodeUID->GetUID(), entryHandle);
		}
		return newNode;
	}
	return {};
}

bool ScriptGraphSchema::RemoveNode(size_t aNodeUID)
{
	ScriptGraph* graph = GetMutableGraph();
	if(!graph->myNodes.contains(aNodeUID))
	{
		return false;
	}

	std::shared_ptr<ScriptGraphNode> node = graph->myNodes.at(aNodeUID);
	for(auto& [id, pin] : node->myPins)
	{
		const std::vector<size_t> pinEdges = pin.GetEdges();
		for(const size_t edgeId : pinEdges)
		{
			RemoveEdge(edgeId);
		}

		graph->myPins.erase(id);
	}

	graph->myNodes.erase(aNodeUID);

	return true;
}

bool ScriptGraphSchema::CanCreateEdge(size_t aSourcePinId, size_t aTargetPinId, std::string& outMessage) const
{
	if(aSourcePinId == aTargetPinId)
		return false;

	const ScriptGraphPin& pinA = GetMutableGraph()->GetPinFromId(aSourcePinId);
	const ScriptGraphPin& pinB = GetMutableGraph()->GetPinFromId(aTargetPinId);

	if(pinA.GetPinDirection() == pinB.GetPinDirection())
	{
		outMessage = "Cannot link a pin to one of the same direction.";
		return false;
	}

	const ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
	const ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

	if(sourcePin.GetUID() == targetPin.GetUID())
	{
		outMessage = "Cannot link a pin to itself.";
		return false;
	}

	const ScriptGraphNode* sourceOwner = sourcePin.GetOwner();
	const ScriptGraphNode* targetOwner = targetPin.GetOwner();

	const ObjectUID* sourceId = AsObjectUIDPtr(sourceOwner);
	const ObjectUID* targetId = AsObjectUIDPtr(targetOwner);

	if(sourceId->GetUID() == targetId->GetUID())
	{
		outMessage = "Cannot link a pin to another pin on the same node.";
		return false;
	}

	bool isCyclic = false;
	CheckCyclicLink(targetPin.GetOwner(), sourcePin.GetOwner(), isCyclic);

	if(isCyclic)
	{
		outMessage = "This connection would result in a cyclic graph loop.";
		return false;
	}

	if(sourcePin.GetPinType() != ScriptGraphPinType::Exec && targetPin.GetPinType() != ScriptGraphPinType::Exec)
	{
		if(sourcePin.GetPinDataType() != targetPin.GetPinDataType())
		{
			outMessage = std::format("{} is not compatible with {}.", sourcePin.GetPinDataType()->GetFriendlyName(), targetPin.GetPinDataType()->GetFriendlyName());
			return false;
		}
	}

	if((sourcePin.GetPinType() == ScriptGraphPinType::Variable || sourcePin.GetPinType() == ScriptGraphPinType::Data)
		&& (targetPin.GetPinType() == ScriptGraphPinType::Variable || targetPin.GetPinType() == ScriptGraphPinType::Data))
	{
		return true;
	}

	return sourcePin.GetPinType() == targetPin.GetPinType();
}

bool ScriptGraphSchema::CreateEdge(size_t aSourcePinId, size_t aTargetPinId)
{
	std::string temp;
	if(CanCreateEdge(aSourcePinId, aTargetPinId, temp))
	{
		ScriptGraphPin& pinA = GetMutablePin(aSourcePinId);
		ScriptGraphPin& pinB = GetMutablePin(aTargetPinId);

		ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
		ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

		// Exec can have only one connection right now. Remove all connections from the pin.
		if(sourcePin.GetPinType() == ScriptGraphPinType::Exec && sourcePin.IsConnected())
		{
			DisconnectPin(sourcePin.GetUID());
		}

		if(targetPin.IsConnected())
		{
			DisconnectPin(targetPin.GetUID());
		}

		CreateEdgeInternal(sourcePin, targetPin);
		return true;
	}

	return false;
}

bool ScriptGraphSchema::DisconnectPin(size_t aPinId)
{
	ScriptGraphPin& pin = GetMutablePin(aPinId);

	const std::vector<size_t> pinEdgeIds = pin.GetEdges();
	for(auto it = pinEdgeIds.begin(); it != pinEdgeIds.end(); ++it)
	{
		RemoveEdge(*it);
	}

	return true;
}

bool ScriptGraphSchema::RemoveEdge(size_t aEdgeId)
{
	ScriptGraph* const graph = GetMutableGraph();
	auto it = graph->myEdges.find(aEdgeId);
	assert(it != graph->myEdges.end());
		
	ScriptGraphPin& fromPin = GetMutablePin(it->second.FromId);
	ScriptGraphPin& toPin = GetMutablePin(it->second.ToId);

	fromPin.RemovePinEdge(aEdgeId);
	toPin.RemovePinEdge(aEdgeId);

	it = graph->myEdges.erase(it);

	return true;
}

void ScriptGraphSchema::MarkDynamicPinForDelete(size_t aPinId)
{
	myPinsToDelete.push_back(aPinId);
	
}

void ScriptGraphSchema::CommitDynamicPinDeletes()
{
	for(const size_t pinId : myPinsToDelete)
	{
		ScriptGraphPin& pin = GetMutablePin(pinId);
		ScriptGraphNode* nodeOwner = pin.myOwner;
		if(pin.IsDynamicPin())
		{
			RemoveDynamicPin(pin.GetOwner(), pinId);
			nodeOwner->OnUserRemovedPin(this, pinId);
		}
	}

	myPinsToDelete.clear();
}

size_t ScriptGraphSchema::CreateDynamicExecPin(size_t aNodeId, std::string_view aLabel, PinDirection aDirection)
{
	ScriptGraph* const graph = GetMutableGraph();
	std::shared_ptr<ScriptGraphNode> node = graph->myNodes.at(aNodeId);
	return CreateDynamicExecPin(node.get(), aLabel, aDirection);
}

size_t ScriptGraphSchema::CreateDynamicExecPin(ScriptGraphNode* aNode, std::string_view aLabel, PinDirection aDirection)
{
	if(!aNode->IsExecNode())
	{
		throw std::runtime_error("Cannot add a Dynamic Exec Pin to a node that has no normal exec pins!");
	}
	const size_t aPinUID = aNode->AddPin(ScriptGraphPin::CreateDynamicPin(aNode, aLabel, ScriptGraphPinType::Exec, aDirection));
	GetMutableGraph()->myPins.emplace(aPinUID, &aNode->GetMutablePin(aPinUID));
	return aPinUID;
}

size_t ScriptGraphSchema::CreateDynamicDataPin(size_t aNodeId, std::string_view aLabel, PinDirection aDirection,
                                             std::type_index aType)
{
	ScriptGraph* const graph = GetMutableGraph();
	std::shared_ptr<ScriptGraphNode> node = graph->myNodes.at(aNodeId);
	return CreateDynamicDataPin(node.get(), aLabel, aDirection, aType);
}

size_t ScriptGraphSchema::CreateDynamicDataPin(ScriptGraphNode* aNode, std::string_view aLabel, PinDirection aDirection,
	std::type_index aType)
{
	const size_t aPinUID = aNode->AddPin(ScriptGraphPin::CreateDynamicDataPin(aNode, aLabel, ScriptGraphPinType::Data, aDirection, aType));
	GetMutableGraph()->myPins.emplace(aPinUID, &aNode->GetMutablePin(aPinUID));
	return aPinUID;
}

void ScriptGraphSchema::RemoveDynamicPin(size_t aNodeId, size_t aPinID)
{
	ScriptGraph* const graph = GetMutableGraph();
	std::shared_ptr<ScriptGraphNode> node = graph->myNodes.at(aNodeId);
	return RemoveDynamicPin(node.get(), aPinID);
}

void ScriptGraphSchema::RemoveDynamicPin(ScriptGraphNode* aNode, size_t aPinID)
{
	const ScriptGraphPin& pin = aNode->GetPin(aPinID);
	if(pin.IsConnected())
	{
		DisconnectPin(aPinID);
	}

	aNode->RemovePin(aPinID);
	GetMutableGraph()->myPins.erase(aPinID);
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddNode(std::type_index aType)
{
	const RegisteredNodeClass& nodeClass = MuninGraph::Get().GetNodeClass(aType);
	return CreateNode(nodeClass);
}

bool ScriptGraphSchema::AddVariable(std::string_view aVariableName, const TypedDataContainer& aDefaultValue, int aFlags)
{
	if(GetGraph()->myVariables.contains(aVariableName))
	{
		return false;
	}

	ScriptGraphVariable newVar;
	newVar.Data = aDefaultValue;
	newVar.Default = aDefaultValue;
	newVar.Name = aVariableName;
	newVar.myFlags = aFlags;

	GetMutableGraph()->myVariables.emplace(aVariableName, std::move(newVar));
	return true;
}

void ScriptGraphSchema::RemoveVariable(const std::string& aVariableName)
{
	// Need to plow through all nodes that use this variable and eat them.
	std::vector<size_t> nodeUIDsToDelete;
	for(const auto& [nodeUID, node] : GetGraph()->myNodes)
	{
		if(const std::shared_ptr<ScriptGraphVariableNode> varNode = std::dynamic_pointer_cast<ScriptGraphVariableNode>(node))
		{
			if(varNode->myVariable->Name == aVariableName)
				nodeUIDsToDelete.push_back(nodeUID);
		}
	}

	for(const size_t& uid : nodeUIDsToDelete)
	{
		RemoveNode(uid);
	}

	GetMutableGraph()->myVariables.erase(aVariableName);
}

void ScriptGraphSchema::SetNodeVariable(size_t aNodeUID, std::string_view aVariableName)
{
	const std::shared_ptr<ScriptGraphNode>& node = GetGraph()->GetNodeById(aNodeUID);
	return SetNodeVariable(node.get(), aVariableName);
}

void ScriptGraphSchema::SetNodeVariable(ScriptGraphNode* aNode, std::string_view aVariableName)
{
	if(ScriptGraphVariableNode* varNode = dynamic_cast<ScriptGraphVariableNode*>(aNode))
	{
		ScriptGraph* graph = GetMutableGraph();
		if(const auto varIt = graph->myVariables.find(aVariableName); varIt != graph->myVariables.end())
		{
			varNode->SetNodeVariable(&varIt->second);
		}		
	}
}

void ScriptGraphSchema::CheckCyclicLink(const ScriptGraphNode* aNode, const ScriptGraphNode* aBaseNode, bool& outResult) const
{
	// TODO: This just checks for cyclic nodes but will prevent functionality similar
	// to UEs ForEachWithBreak nodes because you wouldn't be allowed to connect to the
	// break input exec pin. Ideally this should count pins if it's Exec pins we're
	// looking at instead of just looking at nodes. 

	if(outResult)
	{
		return;
	}

	const auto& baseNodeUID = AsObjectUIDPtr(aBaseNode)->GetUID();
	const auto& pins = aNode->GetPins();
	for(const auto& pin : pins | std::views::values)
	{
		if(pin.GetPinDirection() == PinDirection::Output)
		{
			const auto& edges = pin.GetEdges();
			for(const size_t edgeUID : edges)
			{
				ScriptGraph* graph = GetMutableGraph();
				const ScriptGraphEdge& edge = graph->GetEdgeFromId(edgeUID);
				const auto& targetPin = graph->GetPinFromId(edge.ToId);
				const auto targetNode = targetPin.GetOwner();
				if(AsObjectUIDPtr(targetNode)->GetUID() == baseNodeUID)
				{
					outResult |= true;
					return;
				}
				CheckCyclicLink(targetPin.GetOwner(), aBaseNode, outResult);
			}
		}
	}
}

ScriptGraphPin& ScriptGraphSchema::GetMutablePin(size_t aPinId)
{
	return *GetMutableGraph()->myPins.find(aPinId)->second;
}

void ScriptGraphSchema::CreateEdgeInternal(ScriptGraphPin& aSourcePin, ScriptGraphPin& aTargetPin) const
{
	ScriptGraph* const graph = GetMutableGraph();

	const size_t newEdgeUID = graph->myNextEdgeId++;

	graph->myEdges.insert(
		{
			newEdgeUID,
			{
				{
					newEdgeUID,
				aSourcePin.GetUID(),
				aTargetPin.GetUID()
				},
				aSourcePin.GetPinType()
			}
		});

	aSourcePin.AddPinEdge(newEdgeUID);
	aTargetPin.AddPinEdge(newEdgeUID);

	assert(aTargetPin.GetNumConnections() == 1);
	assert(aSourcePin.GetNumConnections() >= 1);
}
