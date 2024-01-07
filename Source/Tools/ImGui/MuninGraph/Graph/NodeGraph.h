#pragma once
#include "../ScriptGraph/ScriptGraphPin.h" 

//#define DXGI_USAGE_BACK_BUFFER              0x00000040UL
//#define DXGI_USAGE_SHARED                   0x00000080UL
//#define DXGI_USAGE_READ_ONLY                0x00000100UL
//#define DXGI_USAGE_DISCARD_ON_PRESENT       0x00000200UL
//#define DXGI_USAGE_UNORDERED_ACCESS         0x00000400UL
template<typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
class NodeGraph;

template<typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
class NodeGraphInternal : public std::enable_shared_from_this<NodeGraph<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>>
{
	// Our Schema can do whatever it wants.
	friend GraphSchemaType;

	// And the NodeGraph base class can do whatever it wants.
	friend class NodeGraph<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>;

public:
	struct SupportedNodeClass
	{
		std::type_index TypeId = typeid(std::nullptr_t);
		std::type_index BaseTypeId = typeid(GraphNodeType);

		std::function<std::shared_ptr<GraphNodeType>()> New;

		std::string TypeName;
		std::string NodeTitle;
		std::string NodeCategory;
		unsigned NodeFlags = 0;
		unsigned NodeUserflags = 0;

		SupportedNodeClass(const std::type_index& aBaseType);
	};

	static const SupportedNodeClass& GetSupportedNodeClass(std::type_index aType);

private:

	static std::unordered_map<std::string, SupportedNodeClass>& MySupportedNodeTypes()
	{
		static std::unordered_map<std::string, SupportedNodeClass> myMap;
		return myMap;
	}

	static std::unordered_map<std::type_index, std::string>& MySupportedNodeTypeIdToName()
	{
		static std::unordered_map<std::type_index, std::string> myMap;
		return myMap;
	}

public:

	template<typename N, typename B>
	static bool RegisterNodeTypeWithBase(unsigned aNodeFlagList = 0, unsigned aUserFlagList = 0)
	{
		auto tempNodePtr = std::make_shared<N>();
		const auto uuidObj = AsGUIDAwareSharedPtr(tempNodePtr);

		SupportedNodeClass nodeClass(typeid(B));
		nodeClass.TypeId = typeid(N);
		nodeClass.TypeName = uuidObj->GetTypeName();
		nodeClass.NodeTitle = tempNodePtr->GetNodeTitle();
		nodeClass.NodeCategory = tempNodePtr->GetNodeCategory();
		nodeClass.NodeFlags = aNodeFlagList;
		nodeClass.NodeUserflags = aUserFlagList;
		nodeClass.New = []() { auto ptr = std::make_shared<N>(); ptr->Init(); return ptr; };

		MySupportedNodeTypeIdToName().insert({ nodeClass.TypeId, nodeClass.TypeName });
		MySupportedNodeTypes().insert({ nodeClass.TypeName, std::move(nodeClass) });
				
		return true;
	}

	// Only friends can construct us.
	NodeGraphInternal() = default;

	// To make sure all edges in this Graph has unique IDs
	size_t myNextEdgeId = 1;

	// All the nodes that live in this graph.
	std::unordered_map<size_t, std::shared_ptr<GraphNodeType>> myNodes = {};

	// Acceleration map for reverse lookup of PinUID to Pin.
	std::unordered_map<size_t, GraphPinType*> myPins = {};

	// Edge lookup of EdgeUID to Edge.
	std::unordered_map<size_t, GraphEdgeType> myEdges = {};

	FORCEINLINE static const std::unordered_map<std::string, SupportedNodeClass>& GetSupportedNodeTypes()
	{
		return MySupportedNodeTypes();
	}

protected: // These are functions that friends of NodeGraph can access.

	/**
	 * \brief Retrieves the Pin that should be used as the data source when reading from
	 *		  the provided PinUID by checking the EdgeMap.
	 * \param aPinUID The Pin UID to retrieve data for.
	 * \return The GraphPinType reference to the actual data source node.
	 */
	virtual const GraphPinType& GetDataSourcePin(size_t aPinUID, bool& outErrored) const;

	/**
	 * \brief Finds the actual GraphPinType ref from the provided PinUID.
	 * \param aPinUID The UID of the pin to find.
	 * \return The GraphPinType ref of the requested pin.
	 */
	[[nodiscard]] const GraphPinType& GetPinFromUID(size_t aPinUID) const;

	[[nodiscard]] const GraphEdgeType& GetEdgeFromUID(size_t aEdgeUID) const;

	[[nodiscard]] const std::shared_ptr<GraphNodeType>& GetNodeByUID(size_t aNodeUID) const;

public:

	virtual ~NodeGraphInternal() = default;

	NodeGraphInternal(const NodeGraphInternal& anOther);
	NodeGraphInternal& operator=(const NodeGraphInternal& anOther);

	// TODO: Need a more intelligent way to manage lifetime of this object.
	// Should be requested whenever you want to CHANGE the graph and is unique
	// per graph. If you call this it'll create the object with this graph as
	// argument and return it.
	std::unique_ptr<GraphSchemaType> GetGraphSchema();

	[[nodiscard]] const std::unordered_map<size_t, std::shared_ptr<GraphNodeType>>& GetNodes() const { return myNodes; }
	[[nodiscard]] const std::unordered_map<size_t, GraphEdgeType>& GetEdges() const { return myEdges; }
};

/**
 * \brief Base class for Node Graphs.
 * \tparam GraphNodeType Base class for nodes that will exist in this graph.
 * \tparam GraphPinType The class that will be used for node Pins.
 * \tparam GraphEdgeType The struct that will be used for Edges.
 * \tparam GraphSchemaType The schema that governs how this graph functions.
 */
template<class GraphNodeType, class GraphPinType, class GraphEdgeType, class GraphSchemaType>
class NodeGraph : public NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>
{
	// These classes can access Protected items in NodeGraphInternal.
	friend GraphNodeType;
	friend GraphPinType;
	friend GraphEdgeType;

public:

	NodeGraph() = default;
	~NodeGraph() override = default;
};

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::SupportedNodeClass::SupportedNodeClass(
	const std::type_index& aBaseType)
{
	BaseTypeId = aBaseType;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
const typename NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::SupportedNodeClass&
NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::GetSupportedNodeClass(
	std::type_index aType)
{
	if(auto nameIt = MySupportedNodeTypeIdToName().find(aType); nameIt != MySupportedNodeTypeIdToName().end())
	{
		auto typeIt = MySupportedNodeTypes().find(nameIt->second);
		return typeIt->second;
	}

	assert(false && "No such node class !");
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
const GraphPinType& NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::GetDataSourcePin(
	size_t aPinUID, bool& outErrored) const
{
	const GraphPinType& pin = GetPinFromUID(aPinUID);
	const PinDirection pinDir = pin.GetPinDirection(); pinDir;

	assert(pinDir == PinDirection::Input && "Pin Data can only be fetched from Input pins!");

	outErrored = false;

	if (pin.IsPinConnected())
	{
		// Inputs only have one connection.
		const NodeGraphEdge& myEdge = myEdges.find(pin.GetEdges()[0])->second;
		return GetPinFromUID(myEdge.FromUID);
	}

	return pin;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename SchemaType>
const GraphPinType& NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, SchemaType>::GetPinFromUID(size_t aPinUID) const
{
	const auto it = myPins.find(aPinUID);
	assert(it != myPins.end() && "Invalid Pin ID!");
	return *it->second;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
const GraphEdgeType& NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::GetEdgeFromUID(
	size_t aEdgeUID) const
{
	const auto it = myEdges.find(aEdgeUID);
	assert(it != myEdges.end() && "Invalid Edge ID!");
	return it->second;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
const std::shared_ptr<GraphNodeType>& NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::
GetNodeByUID(size_t aNodeUID) const
{
	const auto it = myNodes.find(aNodeUID);
	assert(it != myNodes.end() && "Invalid Node ID!");
	return it->second;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>::NodeGraphInternal(
	const NodeGraphInternal& anOther)
{
	// We should avoid doing too much here since copying should be up to the implementing graph.
	// Therefore the only the data we can safely copy is our next node Id counter.
	// The derived graph should be responsible for reconstructing in its cctor.
	myNextEdgeId = anOther.myNextEdgeId;

	return *this;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename GraphSchemaType>
NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, GraphSchemaType>& NodeGraphInternal<GraphNodeType,
GraphPinType, GraphEdgeType, GraphSchemaType>::operator=(const NodeGraphInternal& anOther)
{
	// Same goes for the cctor. We leave this up to the implementing graph to reconstruct as needed.
	myNextEdgeId = anOther;
}

template <typename GraphNodeType, typename GraphPinType, typename GraphEdgeType, typename SchemaType>
std::unique_ptr<SchemaType> NodeGraphInternal<GraphNodeType, GraphPinType, GraphEdgeType, SchemaType>::GetGraphSchema()
{
	return std::make_unique<SchemaType>(this->shared_from_this());
}
