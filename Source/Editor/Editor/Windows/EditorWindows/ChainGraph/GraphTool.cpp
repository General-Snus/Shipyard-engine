#include "GraphTool.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Tools/ImGui/MuninGraph/MuninGraph.h>
#include <Tools/ImGui/MuninGraphEditor/MuninGraphEditor.h>
#include <Tools/ImGui/MuninGraphEditor/ScriptGraphEditor/ScriptGraphEditor.h>
#include <Tools/Logging/Logging.h>
#include <filesystem>
#include <memory>

std::shared_ptr<ScriptGraphEditor> Graph::GraphTool::OpenScriptingEditor(const std::filesystem::path &AssetPath)
{
    if (myScriptEditors.contains(AssetPath))
    {
        return myScriptEditors[AssetPath];
    }
    else
    {
        std::shared_ptr<GraphAsset> graph = nullptr;
        if (!AssetManager::Get().ForceLoadAsset<GraphAsset>(AssetPath, graph))
        {
            Logger::Err("Failed to load graph asset: " + AssetPath.string());
            graph = std::make_shared<GraphAsset>(AssetPath);
            graph->Init();
        }
        auto editor = std::make_shared<ScriptGraphEditor>(setting, state, graph);
        myScriptEditors[AssetPath] = editor;
        return editor;
    }
}

void Graph::GraphTool::Init()
{
    setting = new ScriptGraphEditorSettings();
    state = new ScriptGraphEditorState();
}

void Graph::GraphTool::RunEditor()
{
    auto ScriptEditor = Graph::GraphTool::OpenScriptingEditor("Autosave.Graph");
    try
    {
        ScriptEditor->Render();
    }
    catch (...)
    {
        Logger::Err("ScriptGraph Crashed Again...");
    }
}
