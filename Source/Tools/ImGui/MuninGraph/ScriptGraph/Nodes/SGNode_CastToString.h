#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_FloatToString)
{
  public:
    void Init() override;
    std::string GetNodeTitle() const override
    {
        return "Float to String";
    }
    std::string GetNodeCategory() const override
    {
        return "Casts";
    }
    size_t DoOperation() override;
    FORCEINLINE bool IsSimpleNode() const override
    {
        return true;
    }
};

BeginScriptGraphNode(SGNode_UUIDToString)
{
  public:
    void Init() override;
    std::string GetNodeTitle() const override
    {
        return "UUID to String";
    }
    std::string GetNodeCategory() const override
    {
        return "Casts";
    }
    size_t DoOperation() override;
    FORCEINLINE bool IsSimpleNode() const override
    {
        return true;
    }
};

BeginScriptGraphNode(SGNode_GameObjectToString)
{
  public:
    void Init() override;
    std::string GetNodeTitle() const override
    {
        return "GameObject to String";
    }
    std::string GetNodeCategory() const override
    {
        return "Casts";
    }
    size_t DoOperation() override;
    FORCEINLINE bool IsSimpleNode() const override
    {
        return true;
    }
};
