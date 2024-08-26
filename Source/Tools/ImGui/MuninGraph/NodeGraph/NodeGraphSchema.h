#pragma once
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include "NodeGraphCommon.h"

#include "UID.h"

struct RegisteredNodeClass;

class GraphSchemaBase
{
	std::type_index mySchemaType;
protected:
	GraphSchemaBase(std::type_index aMyType);
	virtual ~GraphSchemaBase();

	bool IsNodeSupportedBySchema(std::type_index aNodeType) const;
};

/**
 * \brief Base class for new Node-Graph Schemas.
 * \tparam NodeGraphSchemaClass The class you're deriving from NodeGraphSchema, to provide type information to Munin Graph.
 * \tparam NodeGraphClass The class for the Node Graph that this Schema will govern.
 * \tparam NodeClass The base class for Nodes that will live in the Node Graph this Schema governs.
 */
template<typename NodeGraphSchemaClass, typename NodeGraphClass, typename NodeClass, typename NodeEdgeClass, typename NodePinClass>
class NodeGraphSchema : public GraphSchemaBase
{
protected:

	NodeGraphClass* GetMutableGraph() const
	{
		return myGraph;
	}

	bool RegisterNode(const std::shared_ptr<NodeClass>& aNode)
	{
		const std::shared_ptr<const ObjectUID> aNodeUID = AsObjectUIDSharedPtr(aNode);
		if(!IsNodeSupportedBySchema(aNodeUID->GetType()))
		{
			return false;
		}

		if(aNode->HasFlag(GraphNodeFlag_Unique))
		{
			for(const auto& [id, node] : myGraph->myNodes)
			{
				const auto nodeUID = AsObjectUIDSharedPtr(node);
				const std::type_index TypeA = nodeUID->GetType();
				const std::type_index TypeB = aNodeUID->GetType();
				if(nodeUID->GetType() == aNodeUID->GetType())
					return false;
			}
		}

		aNode->myOwner = myGraph;
		myGraph->myNodes.emplace(aNodeUID->GetUID(), aNode);
		for(auto& [id, pin] : aNode->myPins)
		{
			myGraph->myPins.emplace(id, &pin);
		}

		return true;
	}

public:

	//template<typename N>
	//static bool RegisterNodeType()
	//{
	//	static_assert(std::is_base_of_v<NodeClass, N>);
	//	SupportedNodeClass nodeClass;
	//	nodeClass.Type = typeid(N);
	//	nodeClass.Base = typeid(NodeClass);
	//	nodeClass.New = []{ return std::make_shared<N>(); };

	//	// Make the CDO
	//	nodeClass.ClassDefaultObject = nodeClass.New();

	//	// Populate type information.
	//	N* nodeClassPtr = static_cast<N*>(nodeClass.ClassDefaultObject.get());
	//	nodeClass.Name = nodeClassPtr->GetNodeTitle();
	//	nodeClass.Category = nodeClassPtr->GetNodeCategory();

	//	return true;
	//}

	NodeGraphSchema(const NodeGraphSchema&) = delete;
	NodeGraphSchema& operator=(const NodeGraphSchema&) = delete;

	NodeGraphSchema(NodeGraphSchema&&) noexcept = default;
	NodeGraphSchema& operator=(NodeGraphSchema&&) noexcept = default;

	NodeGraphSchema(NodeGraphClass* aGraph)
		: GraphSchemaBase(typeid(NodeGraphSchemaClass)), myGraph(aGraph)
	{  }

	virtual ~NodeGraphSchema()
	{
		// We're not responsible for cleaning up this myGraph.
		myGraph = nullptr;
	}

	[[nodiscard]] const std::unordered_map<size_t, std::shared_ptr<NodeClass>>& GetNodes() const
	{
		return myGraph->myNodes;
	}

	[[nodiscard]] const std::unordered_map<size_t, NodeEdgeClass>& GetEdges() const
	{
		return myGraph->myEdges;
	}

	const NodeGraphClass* GetGraph() const
	{
		return myGraph;
	}

	const NodePinClass* GetPin(size_t aPinId) const
	{
		return myGraph->myPins.at(aPinId);
	}

	[[nodiscard]] const std::unordered_map<size_t, NodePinClass*>& GetPins() const
	{
		return myGraph->myPins;
	}

	virtual std::shared_ptr<NodeClass> CreateNode(const RegisteredNodeClass& aClass) = 0;
	virtual bool RemoveNode(size_t aNodeUID) = 0;

	/**
	 * \brief Checks if the two provided pins can be joined with an edge.
	 * \param aSourcePinUID The From pin UID.
	 * \param aTargetPinUID The Target pin UID.
	 * \param outMessage Feedback providing helpful information regarding the return result.
	 * \return True if an edge can be created.
	 */
	virtual bool CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMessage) const = 0;

	/**
	 * \brief Creates an edge between the two provided pins. This may disconnect existing edges
	 *		  depending on the rules in the schema.
	 * \param aSourcePinUID The From pin UID.
	 * \param aTargetPinUID The To pin UID.
	 * \return True if the edge was successfully created.
	 */
	virtual bool CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID) = 0;

	/**
	 * \brief Removes an existing edge.
	 * \param aEdgeUID The UID of the edge to remove.
	 * \return True if the edge was successfully removed.
	 */
	virtual bool RemoveEdge(size_t aEdgeUID) = 0;

	/**
	 * \brief Removes all edges from the specified pin.
	 * \param aPinUID The UID of the pin to disconnect all links from.
	 * \return True if the pin was successfully disconnected.
	 */
	virtual bool DisconnectPin(size_t aPinUID) = 0;

private:

	NodeGraphClass* myGraph;
	std::unordered_map<std::type_index, size_t> myNodeTypeCounts;
};
