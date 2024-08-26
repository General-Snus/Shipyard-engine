#pragma once

DECLARE_GRAPH_NODE(SGNode_LogText, ScriptGraphNode)
{
public:
	SGNode_LogText();

	NodeResult DoOperation() override;

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Log Text"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Text"; }
};
