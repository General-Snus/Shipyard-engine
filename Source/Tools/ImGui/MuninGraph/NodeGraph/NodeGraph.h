#pragma once
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>

#include "NodeGraphCommon.h"

// #include "NodeGraphPin.h"

template <typename NodeClass, typename NodePinClass, typename NodeEdgeClass, typename NodeGraphSchemaClass>
class NodeGraph;

template <typename NodeClass, typename NodePinClass, typename NodeEdgeClass, typename NodeGraphSchemaClass>
class NodeGraphInternal
{
    // The Schema can do whatever it wants.
    friend NodeGraphSchemaClass;
    template <typename S, typename G, typename N, typename E, typename P> friend class NodeGraphSchema;

    // And the NodeGraph public implementation can do whatever it wants.
    friend NodeGraph<NodeClass, NodePinClass, NodeEdgeClass, NodeGraphSchemaClass>;

  public:
    NodeGraphInternal() = default;
    virtual ~NodeGraphInternal() = default;

    NodeGraphInternal(const NodeGraphInternal &anOther)
    {
        // We should avoid doing too much here since copying should be up to the implementing myGraph.
        // Therefore the only the data we can safely copy is our next node Id counter.
        // The derived myGraph should be responsible for reconstructing in its cctor.
        myNextEdgeId = anOther.myNextEdgeId;
    }

    NodeGraphInternal &operator=(const NodeGraphInternal &anOther)
    {
        // Same goes for the cctor. We leave this up to the implementing myGraph to reconstruct as needed.
        myNextEdgeId = anOther.myNextEdgeId;
        return *this;
    }

    virtual bool Serialize(std::vector<uint8_t> &outResult);
    virtual bool Deserialize(const std::vector<uint8_t> &inData);

  protected:
    [[nodiscard]] const NodePinClass &GetPinFromId(size_t aPinId) const
    {
        const auto it = myPins.find(aPinId);
        if (it == myPins.end())
        {
            const std::string msg = std::format("Could not find a pin with the ID {}!", aPinId);
            throw std::runtime_error(msg);
        }

        return *it->second;
    }
    [[nodiscard]] const NodeEdgeClass &GetEdgeFromId(size_t aEdgeId) const
    {
        const auto it = myEdges.find(aEdgeId);
        if (it == myEdges.end())
        {
            const std::string msg = std::format("Could not find an edge with the ID {}!", aEdgeId);
            throw std::runtime_error(msg);
        }

        return it->second;
    }
    [[nodiscard]] const std::shared_ptr<NodeClass> &GetNodeById(size_t aNodeId) const
    {
        const auto it = myNodes.find(aNodeId);
        if (it == myNodes.end())
        {
            const std::string msg = std::format("Could not find a node with the ID {}!", aNodeId);
            throw std::runtime_error(msg);
        }

        return it->second;
    }

    FORCEINLINE const std::unordered_map<size_t, std::shared_ptr<NodeClass>> &GetNodes() const
    {
        return myNodes;
    };
    FORCEINLINE const std::unordered_map<size_t, NodePinClass *> &GetPins() const
    {
        return myPins;
    };
    FORCEINLINE const std::unordered_map<size_t, NodeEdgeClass> &GetEdges() const
    {
        return myEdges;
    };

    virtual void Reset()
    {
        myEdges.clear();
        myPins.clear();
        myNodes.clear();
    }

  private:
    size_t myNextEdgeId = 1;
    std::unordered_map<size_t, std::shared_ptr<NodeClass>> myNodes = {};
    std::unordered_map<size_t, NodePinClass *> myPins = {};
    std::unordered_map<size_t, NodeEdgeClass> myEdges = {};
};

template <typename NodeClass, typename NodePinClass, typename NodeEdgeClass, typename NodeGraphSchemaClass>
bool NodeGraphInternal<NodeClass, NodePinClass, NodeEdgeClass, NodeGraphSchemaClass>::Serialize(
    std::vector<uint8_t> &outResult)
{
    outResult;
    throw std::runtime_error("This myGraph has not implemented Serialize!");
}

template <typename NodeClass, typename NodePinClass, typename NodeEdgeClass, typename NodeGraphSchemaClass>
bool NodeGraphInternal<NodeClass, NodePinClass, NodeEdgeClass, NodeGraphSchemaClass>::Deserialize(
    const std::vector<uint8_t> &inData)
{
    inData;
    throw std::runtime_error("This myGraph has not implemented Deserialize!");
}

template <typename NodeClass, typename NodePinClass, typename NodeEdgeClass, typename NodeGraphSchemaClass>
class NodeGraph : public NodeGraphInternal<NodeClass, NodePinClass, NodeEdgeClass, NodeGraphSchemaClass>
{
    // These classes can access Protected items in NodeGraphInternal.
    // By defining protected members in the base class friends defined
    // here can then access them but nothing else can aside from expected
    // class hierarchy.
    friend NodeClass;
    friend NodePinClass;
    friend NodeEdgeClass;

    // The Schema can do whatever it wants.
    friend NodeGraphSchemaClass;
    template <typename S, typename G, typename N, typename E, typename P> friend class NodeGraphSchema;

  public:
    NodeGraph() = default;
    ~NodeGraph() override = default;
};
