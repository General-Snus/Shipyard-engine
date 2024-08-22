#pragma once
#include "SGNode_EventBase.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphDerivedNode(SGNode_EventTick, SGNode_EventBase)
{
  public:
    void Init() override;

    std::string GetNodeTitle() const override
    {
        return "Tick";
    }
    std::string GetDescription() const override
    {
        return "An event node that fires every frame.";
    };
};

BeginScriptGraphDerivedNode(SGNode_EventOnEnter, SGNode_EventBase)
{
  public:
    void Init() override;

    std::string GetNodeTitle() const override
    {
        return "OnEnter";
    }
};

BeginScriptGraphDerivedNode(SGNode_EventOnExit, SGNode_EventBase)
{
  public:
    void Init() override;

    std::string GetNodeTitle() const override
    {
        return "OnExit";
    }
};
