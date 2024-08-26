#pragma once
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Sequence, ScriptGraphNode)
{
  public:
    SGNode_Sequence();

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Sequence";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Flow Control";
    }

    NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;

#ifndef NOEDITOR
    void OnUserAddedPin(ScriptGraphSchema * aSchema, size_t aPinId) override;
    void OnUserRemovedPin(ScriptGraphSchema * aSchema, size_t aPinId) override;
#endif

  private:
    void RefreshExitPins();

    size_t myIndex = 0;
    std::vector<size_t> myExitPins;
};
