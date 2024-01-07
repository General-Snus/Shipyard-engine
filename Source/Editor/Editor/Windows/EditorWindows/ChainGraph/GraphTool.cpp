#include "GraphTool.h"

std::shared_ptr<ScriptGraphEditor> Graph::GraphTool::GetScriptingEditor()
{
    return std::make_shared<ScriptGraphEditor>();
}

Graph::GraphTool::GraphTool()
{
}

Graph::GraphTool::~GraphTool()
{

}
