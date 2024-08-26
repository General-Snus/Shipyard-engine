#include "ScriptGraph.h"

#include <ranges>

#include "ScriptGraphEdge.h"
#include "ScriptGraphNode.h"
#include "ScriptGraphPayload.h"
#include "ScriptGraphPin.h"
#include "ScriptGraphSchema.h"
#include "ScriptGraphVariable.h"

#include "Internal/json.hpp"
#include "Nodes/VariableNodes.h"

ScriptGraph::ScriptGraph(const std::filesystem::path &graph, void *aOwner)
    : myShouldStop(false), myNumActiveFunctions(0), myOwner(aOwner), m_GraphAssetPath(graph)
{
    // Create the default things here.
    SetupScriptGraph();
}

bool ScriptGraph::Execute(std::string_view aEntryPointHandle)
{
    bool executeResult = false;
    if (const auto &it = myEntryPoints.find(aEntryPointHandle); it != myEntryPoints.end())
    {
        if (const auto &node = GetNodeById(it->second))
        {
            myLastExecutedPath.clear();
            myShouldStop = false;
            myNumActiveFunctions++;
            executeResult = ExecuteInternal(node.get(), 0);
            myNumActiveFunctions--;
        }
    }

    return executeResult;
}

bool ScriptGraph::ExecuteWithPayload(std::string_view aEntryPointHandle, const ScriptGraphPayload &aPayload)
{
    if (const auto &it = myEntryPoints.find(aEntryPointHandle); it != myEntryPoints.end())
    {
        if (const auto &node = GetNodeById(it->second))
        {
            node->DeliverPayload(aPayload);
            return Execute(aEntryPointHandle);
        }
    }

    return false;
}

void ScriptGraph::Tick(float aDeltaTime)
{
    myLastExecutedPath.clear();
    static ScriptGraphPayload thisPayload;
    thisPayload.SetPinValue("Delta Time", aDeltaTime);
    ExecuteWithPayload("Tick", thisPayload);
}

void ScriptGraph::Stop()
{
    myShouldStop = true;
}

bool ScriptGraph::Serialize(std::vector<uint8_t> &outResult)
{
    using namespace nlohmann;

    json graphJson;
    graphJson["variables"] = json::array();

    for (const auto &variable : myVariables | std::views::values)
    {
        if (variable.HasFlag(ScriptGraphVariableFlag_Internal))
            continue;

        json varJson;
        varJson["name"] = variable.Name;
        varJson["type"] = variable.GetType()->GetFriendlyName();
        varJson["flags"] = variable.GetFlags();

        std::vector<uint8_t> varData;
        variable.Default.Serialize(varData);
        varJson["value"] = varData;
        graphJson["variables"].emplace_back(varJson);
    }

    graphJson["nodes"] = json::array();

    for (const auto &[nodeId, node] : GetNodes())
    {
        const auto &objectUIDNode = AsObjectUIDSharedPtr(node);

        json nodeJson;
        nodeJson["type"] = objectUIDNode->GetTypeName();
        nodeJson["id"] = nodeId;
        json nodePositionJson;
        float X, Y, Z;
        node->GetNodePosition(X, Y, Z);
        nodePositionJson["X"] = X;
        nodePositionJson["Y"] = Y;
        nodePositionJson["Z"] = Z;
        nodeJson["position"] = nodePositionJson;

        const ScriptGraphNodeType nodeType = node->GetNodeType();
        if (nodeType == ScriptGraphNodeType::Variable)
        {
            const std::shared_ptr<ScriptGraphVariableNode> varNodeBase =
                std::dynamic_pointer_cast<ScriptGraphVariableNode>(node);
            nodeJson["variable"] = varNodeBase->GetVariable()->Name;
        }

        nodeJson["pins"] = json::array();

        for (const auto &pin : node->GetPins() | std::views::values)
        {
            if (!pin.IsDynamicPin() && pin.GetPinType() == ScriptGraphPinType::Exec)
                continue;

            json pinJson;
            pinJson["name"] = pin.GetLabel();
            pinJson["dynamic"] = pin.IsDynamicPin();

            if (pin.IsDynamicPin())
            {
                pinJson["type"] = pin.GetPinType() == ScriptGraphPinType::Exec ? true : false;
                pinJson["direction"] = pin.GetPinDirection() == PinDirection::Input ? true : false;
                if (pin.GetPinType() == ScriptGraphPinType::Data)
                {
                    const RegisteredType *pinDataType = pin.GetPinDataType();
                    pinJson["data"] = pinDataType->GetFriendlyName();
                }
            }

            if (pin.GetPinType() == ScriptGraphPinType::Data && pin.GetPinDirection() == PinDirection::Input)
            {
                std::vector<uint8_t> pinData;
                pin.GetDataContainer().Serialize(pinData);
                pinJson["value"] = std::move(pinData);
            }

            nodeJson["pins"].emplace_back(pinJson);
        }

        graphJson["nodes"].emplace_back(nodeJson);
    }

    graphJson["edges"] = json::array();

    for (const auto &[edgeId, edge] : GetEdges())
    {
        json edgeJson;
        edgeJson["id"] = edgeId;

        const ScriptGraphPin &fromPin = GetPinFromId(edge.FromId);
        const ScriptGraphPin &toPin = GetPinFromId(edge.ToId);
        const auto fromNodeId = AsObjectUIDPtr(fromPin.GetOwner());
        const auto toNodeId = AsObjectUIDPtr(toPin.GetOwner());

        edgeJson["sourcePin"] = fromPin.GetLabel();
        edgeJson["sourceNode"] = fromNodeId->GetUID();
        edgeJson["targetPin"] = toPin.GetLabel();
        edgeJson["targetNode"] = toNodeId->GetUID();
        graphJson["edges"].emplace_back(edgeJson);
    }

    outResult = json::to_bson(graphJson);

    return true;
}

bool ScriptGraph::Deserialize(const std::vector<uint8_t> &inData)
{

    // Nuke the existing graph data.
    Reset();

    ScriptGraphSchema schema(this);

    // To handle conversions from file UIDs to actual UIDs;
    std::unordered_map<size_t, std::shared_ptr<ScriptGraphNode>> fileUIDToNode;
    std::unordered_map<size_t, size_t> pinUIDToNode;
    std::vector<std::string> varNames;

    using namespace nlohmann;

    json graphJson = json::from_bson(inData);
    if (graphJson.contains("variables"))
    {
        for (const json &varJson : graphJson["variables"])
        {
            const std::string varName = varJson["name"];
            const std::string varType = varJson["type"];
            int varFlags = varJson["flags"];
            std::vector<uint8_t> varData = varJson["value"];
            const TypedDataContainer varValue = TypedDataContainer::Create(TypeRegistry::Get().Resolve(varType));
            varValue.Deserialize(varData);
            schema.AddVariable(varName, varValue, varFlags);
            varNames.emplace_back(varName);
        }
    }

    for (const json &nodeJson : graphJson["nodes"])
    {
        const RegisteredNodeClass &nodeClass = MuninGraph::Get().GetNodeClass(nodeJson["type"]);
        auto newNode = schema.AddNode(nodeClass.Type);

        const float X = nodeJson["position"].at("X");
        const float Y = nodeJson["position"].at("Y");
        const float Z = nodeJson["position"].at("Z");
        newNode->SetNodePosition(X, Y, Z);

        const ScriptGraphNodeType nodeType = nodeClass.GetCDO<ScriptGraphNode>()->GetNodeType();
        if (nodeType == ScriptGraphNodeType::Variable)
        {
            schema.SetNodeVariable(newNode.get(), nodeJson["variable"]);
        }

        for (const auto &pinJson : nodeJson.at("pins"))
        {
            const std::string &pinName = pinJson.at("name");
            const bool pinIsDynamic = pinJson.at("dynamic");

            if (pinIsDynamic)
            {
                const bool isExec = pinJson.at("type");
                const bool isInput = pinJson.at("direction");
                if (isExec)
                {
                    schema.CreateDynamicExecPin(newNode.get(), pinName,
                                                isInput ? PinDirection::Input : PinDirection::Output);
                }
                else
                {
                    const std::string pinDataTypeName = pinJson.at("data");
                    const RegisteredType *pinDataType = TypeRegistry::Get().Resolve(pinDataTypeName);
                    schema.CreateDynamicDataPin(newNode.get(), pinName,
                                                isInput ? PinDirection::Input : PinDirection::Output,
                                                pinDataType->GetType());
                }
            }

            if (pinJson.contains("value"))
            {
                const std::vector<uint8_t> &pinValue = pinJson.at("value");
                if (const ScriptGraphPin &nodePin = newNode->GetPin(pinName))
                {
                    nodePin.GetDataContainer().Deserialize(pinValue);
                }
            }
        }

        fileUIDToNode.emplace(nodeJson.at("id"), newNode);
    }

    if (graphJson.contains("edges"))
    {
        for (const json &edgeJson : graphJson["edges"])
        {
            const std::shared_ptr<ScriptGraphNode> &sourceNode = fileUIDToNode.at(edgeJson.at("sourceNode"));
            const std::shared_ptr<ScriptGraphNode> &targetNode = fileUIDToNode.at(edgeJson.at("targetNode"));

            const std::string &sourcePinName = edgeJson.at("sourcePin");
            const ScriptGraphPin &sourcePin = sourceNode->GetPin(sourcePinName);

            const std::string &targetPinName = edgeJson.at("targetPin");
            const ScriptGraphPin &targetPin = targetNode->GetPin(targetPinName);

            schema.CreateEdge(sourcePin.GetUID(), targetPin.GetUID());
        }
    }

    return true;
}

void ScriptGraph::BindErrorHandler(ScriptGraphErrorHandlerSignature &&aErrorHandler)
{
    myErrorDelegate = std::move(aErrorHandler);
}

void ScriptGraph::UnbindErrorHandler()
{
    myErrorDelegate = nullptr;
}

void ScriptGraph::Reset()
{
    NodeGraph::Reset();
    myVariables.clear();
    myEntryPoints.clear();
    myNodeIdToEntryPoint.clear();
    SetupScriptGraph();
}

void ScriptGraph::SetupScriptGraph()
{
    ScriptGraphSchema schema(this);
    const TypedDataContainer container = TypedDataContainer::Create(typeid(myOwner));
    schema.AddVariable("Owner", container, ScriptGraphVariableFlag_Internal | ScriptGraphVariableFlag_ReadOnly);
}

bool ScriptGraph::ExecuteInternal(ScriptGraphNode *aNode, size_t aPinId)
{
    assert(aNode);

    ScriptGraphNode *currentNode = aNode;
    size_t currentNodeEntryPin = aPinId;

    bool railFinished = false;

    NodeResult result;
    do
    {
        // Execute the node.
        result = currentNode->Enter(currentNodeEntryPin, NodeEntryType::New);

        // If the node is In Progress, just loop and let it do its thing.
        // This is used in i.e. WHILE, FOR and FOREACH loops or even SEQUENCE nodes.
        while (result.State() == NodeResultState::InProgress && myShouldStop == false)
        {
            const ScriptGraphPin &exitPin = GetPinFromId(result.Data());
            if (exitPin.IsConnected())
            {
                const size_t exitEdgeId = exitPin.GetEdges()[0];
                const NodeGraphEdge &exitEdge = GetEdgeFromId(exitEdgeId);
                const ScriptGraphPin &entryPin = GetPinFromId(exitEdge.ToId);

                myLastExecutedPath.push_back(exitEdgeId);

                // Start a new Exec for the provided pin.
                if (!ExecuteInternal(entryPin.GetOwner(), entryPin.GetUID()))
                {
                    // If something went wrong, stop executing.
                    return false;
                }
            }

            // Then tell the node to execute again.
            result = currentNode->Enter(aPinId, NodeEntryType::InProgress);
        }

        // Result was Finished, go to next node if we have one.
        if (result.State() == NodeResultState::Finished)
        {
            if (result.Data() > 0)
            {
                const ScriptGraphPin &exitPin = GetPinFromId(result.Data());
                railFinished = !exitPin.IsConnected();
                if (!railFinished)
                {
                    const size_t exitEdgeId = exitPin.GetEdges()[0];
                    const NodeGraphEdge &exitEdge = GetEdgeFromId(exitEdgeId);
                    const ScriptGraphPin &entryPin = GetPinFromId(exitEdge.ToId);

                    myLastExecutedPath.push_back(exitEdgeId);

                    currentNode = entryPin.GetOwner();
                    currentNodeEntryPin = entryPin.GetUID();
                }
            }
        }

    } while (!railFinished && result.State() != NodeResultState::Error && myShouldStop == false);

    if (result.State() == NodeResultState::Error)
    {
        const auto nodeUIDPtr = AsObjectUIDPtr(currentNode);
        ReportError(nodeUIDPtr->GetUID(), currentNode->GetError());
        return false;
    }

    assert(result.State() == NodeResultState::Finished);

    return true;
}

void ScriptGraph::ReportError(size_t aNodeId, std::string_view aErrorMessage) const
{
    if (myErrorDelegate)
    {
        myErrorDelegate(this, aNodeId, aErrorMessage);
    }
}
