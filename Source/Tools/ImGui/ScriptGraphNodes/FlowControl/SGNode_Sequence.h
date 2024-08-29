#pragma once
#include "Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraphNode.h"
#include <chrono>

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

DECLARE_GRAPH_NODE(MVNode_Branch, ScriptGraphNode)
{
  public:
    MVNode_Branch();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Branch";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Flow Control";
    }
};

DECLARE_GRAPH_NODE(MVNode_IsKeyPressed, ScriptGraphNode)
{
  public:
    MVNode_IsKeyPressed();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "IsKeyPressed";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Input";
    }
};

DECLARE_GRAPH_NODE(MVNode_ForLoop, ScriptGraphNode)
{
  public:
    MVNode_ForLoop();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "For";
    }
    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Flow Control";
    }
};

DECLARE_GRAPH_NODE(MVNode_Timer, ScriptGraphNode)
{
  public:
    MVNode_Timer();
    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Timer";
    }

    NodeResult DoOperation() override;
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Flow Control";
    }

  private:
    std::chrono::time_point<std::chrono::system_clock> endTime{};
    int sleepTimer{};
};
