#pragma once
#include <vector>

DECLARE_GRAPH_NODE(SGNode_FormatText, ScriptGraphNode)
{
  public:
    SGNode_FormatText();

    NodeResult DoOperation() override;

    FORCEINLINE std::string_view GetNodeTitle() const override
    {
        return "Format Text";
    }
    FORCEINLINE std::string_view GetNodeCategory() const override
    {
        return "Text";
    }

#ifndef NOEDITOR
    void OnUserChangedPinValue(ScriptGraphSchema * aSchema, size_t aPinId) override;
#endif

  private:
    std::vector<std::string> GetFormatTokens(const std::string &aString) const;
};
