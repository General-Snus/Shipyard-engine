#pragma once
#include "Internal/MuninGraphCommon.h"
#include "MuninGraphImpl.h"
#include "NodeGraphCommon.h"
#include "Types/TypeRegistry.h"
#include <format>
#include <unordered_map>

// #include "winnt.h"
//	DEFINE_ENUM_FLAG_OPERATORS()

template <class NodeClass, NodeClassFlags Flags> struct NodeClassAutoRegister
{
    NodeClassAutoRegister()
    {
        MuninGraph::Get().RegisterNodeClass([this] { return GetNodeClass(); });
    }

    RegisteredNodeClass GetNodeClass()
    {
        RegisteredNodeClass nodeClass;
        nodeClass.Type = typeid(NodeClass);
        nodeClass.Base = typeid(NodeClass);
        nodeClass.Flags = Flags;
        nodeClass.New = [] { return std::make_shared<NodeClass>(); };

        // Make the CDO
        nodeClass.ClassDefaultObject = nodeClass.New();

        std::shared_ptr<NodeClass> CDO = std::static_pointer_cast<NodeClass>(nodeClass.ClassDefaultObject);
        const auto &pins = CDO->GetPins();

        if (pins.size() == 0)
        {
            throw std::runtime_error("The defined node class has no pins!");
        }

        nodeClass.TypeName = CDO->GetTypeName();

        // Populate type information.
        nodeClass.Name = CDO->GetNodeTitle();
        nodeClass.Category = CDO->GetNodeCategory();
        nodeClass.SupportedSchemas = CDO->GetSupportedSchemas();

        return nodeClass;
    }
};

template <typename NodePinClass, typename NodeGraphClass, typename NodeGraphSchemaClass> class NodeGraphNode
{
    // Schema can do whatever it wants to us.
    friend NodeGraphSchemaClass;
    template <typename S, typename G, typename N, typename E, typename P> friend class NodeGraphSchema;

  protected:
    // Node internal functions.

    virtual const NodePinClass &AddPin(NodePinClass &&aPin)
    {
        assert(!myPinLabelToPinID.contains(aPin.GetLabel()));
        myPinLabelToPinID.emplace(aPin.GetLabel(), aPin.GetUID());
        size_t pinUID = aPin.GetUID();
        myPins.emplace(pinUID, std::move(aPin));
        return myPins.at(pinUID);
    }

    virtual void RemovePin(size_t aPinID)
    {
        if (myPins.contains(aPinID))
        {
            NodePinClass &pin = myPins.at(aPinID);
            auto it = myPinLabelToPinID.find(pin.GetLabel());
            myPinLabelToPinID.erase(it);
            myPins.erase(aPinID);
        }
    }

    virtual bool RenamePin(std::string_view aPinLabel, std::string_view aNewLabel)
    {
        if (const auto pinLabelIt = myPinLabelToPinID.find(aPinLabel); pinLabelIt != myPinLabelToPinID.end())
        {
            return RenamePin(pinLabelIt->second, aNewLabel);
        }

        return false;
    }

    virtual bool RenamePin(size_t aPinId, std::string_view aNewLabel)
    {
        if (auto pinIt = myPins.find(aPinId); pinIt != myPins.end())
        {
            assert(!myPinLabelToPinID.contains(aNewLabel));
            myPinLabelToPinID.erase(myPinLabelToPinID.find(pinIt->second.GetLabel()));
            pinIt->second.SetLabel(aNewLabel);
            myPinLabelToPinID.emplace(aNewLabel, pinIt->second.GetUID());
            return true;
        }

        return false;
    }

    void AddFlag(GraphNodeFlags aFlag);

    NodePinClass &GetMutablePin(size_t aPinId)
    {
        return myPins.at(aPinId);
    }

  public:
    virtual ~NodeGraphNode() = default;

    FORCEINLINE const NodeGraphClass *GetOwner() const
    {
        return myOwner;
    }
    FORCEINLINE const NodePinClass &GetPin(size_t aPinID) const
    {
        const auto it = myPins.find(aPinID);
        if (it == myPins.end())
        {
            const std::string msg = std::format("Could not find a pin with the ID {}!", aPinID);
            throw std::runtime_error(msg);
        }

        return it->second;
    }
    FORCEINLINE const NodePinClass &GetPin(std::string_view aPinLabel) const
    {
        const auto it = myPinLabelToPinID.find(aPinLabel);
        if (it == myPinLabelToPinID.end())
        {
            const std::string msg = std::format("Could not find a pin with the name {}!", aPinLabel.data());
            throw std::runtime_error(msg);
        }

        return GetPin(it->second);
    }
    FORCEINLINE const std::unordered_map<size_t, NodePinClass> &GetPins() const
    {
        return myPins;
    }

    virtual const std::vector<std::type_index> &GetSupportedSchemas() const = 0;

    virtual FORCEINLINE std::string_view GetNodeTitle() const = 0;
    virtual FORCEINLINE std::string_view GetNodeCategory() const = 0;
    FORCEINLINE bool HasFlag(GraphNodeFlags aFlag) const
    {
        return (myFlags & aFlag);
    }

    // These are here because ImNodeEd has its own position property.
    void SetNodePosition(float x, float y, float z)
    {
        myPosition[0] = x;
        myPosition[1] = y;
        myPosition[2] = z;
    }

    // These are here because ImNodeEd has its own position property.
    void GetNodePosition(float &outX, float &outY, float &outZ) const
    {
        outX = myPosition[0];
        outY = myPosition[1];
        outZ = myPosition[2];
    }

  private:
    std::unordered_map<size_t, NodePinClass> myPins;
    std::unordered_map<std::string, size_t, string_hash, std::equal_to<>> myPinLabelToPinID;

    const NodeGraphClass *myOwner = nullptr;

    float myPosition[3] = {0, 0, 0};

    GraphNodeFlags myFlags = 0;
};

template <typename NodePinClass, typename NodeGraphClass, typename NodeGraphSchemaClass>
void NodeGraphNode<NodePinClass, NodeGraphClass, NodeGraphSchemaClass>::AddFlag(GraphNodeFlags aFlag)
{
    if (!HasFlag(aFlag))
        myFlags |= aFlag;
}

#define DECLARE_GRAPH_NODE(NodeClass, NodeBase)                                                                        \
    __pragma(comment(linker, "/include:GN_" #NodeBase "_" #NodeClass)) class NodeClass                                 \
        : public NodeBase,                                                                                             \
          public TypeObjectUID<NodeClass>

#define IMPLEMENT_GRAPH_NODE(NodeClass, NodeBase)                                                                      \
    static NodeClassAutoRegister<NodeClass, CF_None> NodeRegistrant_##NodeBase##_##NodeClass;                          \
    extern "C" void GN_##NodeBase##_##NodeClass()                                                                      \
    {                                                                                                                  \
    }

#define IMPLEMENT_GRAPH_NODE_EX(NodeClass, NodeBase, ClassFlags)                                                       \
    static NodeClassAutoRegister<NodeClass, ClassFlags> NodeRegistrant_##NodeBase##_##NodeClass;                       \
    extern "C" void GN_##NodeBase##_##NodeClass()                                                                      \
    {                                                                                                                  \
    }
