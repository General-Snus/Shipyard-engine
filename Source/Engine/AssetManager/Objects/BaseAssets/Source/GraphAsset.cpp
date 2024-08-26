#include "Engine/AssetManager/AssetManager.pch.h"

#include "../GraphAsset.h"
#include <Tools/ImGui/MuninGraph/MuninGraph.h>
#include <Tools/ImGui/MuninGraphEditor/MuninGraphEditor.h>

#include <Editor/Editor/Windows/EditorWindows/ChainGraph/GraphTool.h>
#include <stdint.h>
GraphAsset::GraphAsset(const std::filesystem::path &aFilePath) : AssetBase(aFilePath)
{
}

void GraphAsset::Init()
{
    graphToEdit = std::make_shared<ScriptGraph>(AssetPath);

    if (!std::filesystem::exists(AssetPath))
    {
        Logger::Warn("GraphAsset does not exist: " + AssetPath.string());
        return;
    }

    std::ifstream file(AssetPath.string(), std::ios::binary);
    if (file.is_open())
    {
        auto content = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        graphToEdit->Deserialize(content);
        file.close();
    }
    else
    {
        Logger::Err("Failed to load graph: " + AssetPath.string());
        return;
    }
    isLoadedComplete = true;
}

bool GraphAsset::InspectorView()
{
    // if (!AssetBase::InspectorView())
    // {
    //     return false;
    // }
    // Reflect<GraphAsset>();

    if (!isLoadedComplete)
    {
        return false;
    }

    auto ScriptEditor = Graph::GraphTool::OpenScriptingEditor(AssetPath);

    // Brackets of trust issue
    try
    {
        ScriptEditor->Render();
    }
    catch (...)
    {
        Logger::Err("ScriptGraph Crashed Again...");
    }

    return false;
}

std::shared_ptr<TextureHolder> GraphAsset::GetEditorIcon()
{
    // TODO CUSTOM WIDGET
    const auto file = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");

    if (file)
    {
        return file;
    }
    else
    {
        return AssetBase::GetEditorIcon();
    }
}
