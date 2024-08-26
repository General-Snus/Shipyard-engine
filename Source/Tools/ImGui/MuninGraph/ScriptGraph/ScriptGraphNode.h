#pragma once
#include "ScriptGraph.h"
#include "ScriptGraphCommon.h"
#include "ScriptGraphPin.h"
#include "Tools/ImGui/MuninGraph/NodeGraph/NodeGraphCommon.h"
#include "Tools/ImGui/MuninGraph/NodeGraph/NodeGraphNode.h"

class ScriptGraphPayload;
class ScriptGraphSchema;
class ScriptGraph;
class ScriptGraphPin;

class ScriptGraphNode : public NodeGraphNode<ScriptGraphPin, ScriptGraph, ScriptGraphSchema>
{
    friend ScriptGraphSchema;

  public:
    virtual NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType);
    virtual NodeResult DoOperation()
    {
        return NoExec();
    }

    const std::vector<std::type_index> &GetSupportedSchemas() const override;

    ~ScriptGraphNode() override = default;

    FORCEINLINE bool IsExecNode() const
    {
        return myIsExecNode;
    }
    FORCEINLINE std::string_view GetError() const
    {
        return myErrorMessage;
    }

    virtual FORCEINLINE bool IsInternalOnly() const
    {
        return false;
    }
    virtual FORCEINLINE ScriptGraphNodeType GetNodeType() const
    {
        return ScriptGraphNodeType::Function;
    }

    void DeliverPayload(const ScriptGraphPayload &aPayload);

  protected:
    void CreateExecPin(std::string_view aLabel, PinDirection aDirection, bool showLabelOnNode = true);
    void CreateDataPin(std::string_view aLabel, PinDirection aDirection, std::type_index aType,
                       bool hideLabelOnNode = false);

    template <typename T>
    void CreateDataPin(std::string_view aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
    {
        // AddPin(ScriptGraphPin::CreateDataPin(this, aLabel, ScriptGraphPinType::Data, aDirection, typeid(T),
        // hideLabelOnNode));
        CreateDataPin(aLabel, aDirection, typeid(T), hideLabelOnNode);
    }

    void RemovePin(size_t aPinID) override;

    // Return this when the node has no Exec pins.
    NodeResult NoExec();

    // Call this to send execution out on a specific pin.
    // This can be done multiple times, i.e. if you have a loop.
    // Needs to be done at least once on an Exec node to continue the myGraph.
    virtual NodeResult ExecPin(std::string_view aPinLabel, NodeResultState aState = NodeResultState::Finished);

    // Return this if something goes wrong.
    virtual NodeResult Error(std::string_view aErrorMessage);

    void ClearError();

    template <typename T> bool GetPinData(std::string_view aPinLabel, T &outData) const
    {
        const ScriptGraphPin &pin = GetPin(aPinLabel);
        if (pin.IsConnected())
        {
            bool isFound = false;
            const ScriptGraphPin &actualPin = GetDataSourcePin(pin.GetUID(), isFound);
            if (!isFound)
                return false;

            if (actualPin.GetUID() != pin.GetUID() && !actualPin.GetOwner()->IsExecNode())
            {
                // If this node isn't an exec node we need to ask it to
                // evaluate itself so we can get the data we want.
                // Just like UE we force the node to recalculate and we
                // do not cache any result.

                // ScriptGraph design says that this node should not return
                // a pin to continue on since it's not an exec node.
                // Therefore we don't need to take care of that part.
                // This should recursively call GetDataSourcePin as needed.
                const NodeResult nodeResult = actualPin.GetOwner()->DoOperation();
                if (nodeResult.State() == NodeResultState::Error)
                    return false;
            }

            return actualPin.TryGetValue(outData);
        }

        return pin.TryGetValue(outData);
    }

    template <typename T> void SetPinData(std::string_view aPinLabel, const T &aData)
    {
        const ScriptGraphPin &pin = GetPin(aPinLabel);
        pin.TrySetValue(aData);
    }

    bool ChangePinDataType(size_t aPinId, const RegisteredType *aType);

    // Finds the correct pin to read data from by checking the connections of Pin.
    // Will return Pin if no connections are found, or the source Pin if there is
    // a connection.
    virtual const ScriptGraphPin &GetDataSourcePin(size_t aPinId, bool &outFound) const;

  public:
    // These are functions triggered by the editor. Not needed in runtime.
    // Since people seem afraid of macro defines to include things we'll
    // just have a macro define to avoid including things x)
#ifndef NOEDITOR
    //~ Begin User Functions
    virtual void OnUserChangedPinValue(ScriptGraphSchema *aSchema, size_t aPinId);
    virtual void OnUserAddedPin(ScriptGraphSchema *aSchema, size_t aPinId);
    virtual void OnUserRemovedPin(ScriptGraphSchema *aSchema, size_t aPinId);
    //~
#endif

  private:
    // Node state variables
    bool myHasError = false;
    bool myIsExecNode = false;
    std::string myErrorMessage;
};
