#include "GraphTool.h"

std::shared_ptr<ScriptGraphEditor> Graph::GraphTool::OpenScriptingEditor(const std::filesystem::path &AssetPath)
{

    if (myScriptEditors.find(AssetPath) != myScriptEditors.end())
    {
        return myScriptEditors[AssetPath];
    }
    else
    {
        auto editor = std::make_shared<ScriptGraphEditor>();
        myScriptEditors[AssetPath] = editor;
        return editor;
    }
}

Graph::GraphTool::GraphTool()
{
}

Graph::GraphTool::~GraphTool()
{
}
