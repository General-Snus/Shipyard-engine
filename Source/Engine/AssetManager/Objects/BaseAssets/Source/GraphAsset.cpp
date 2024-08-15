#include "Engine/AssetManager/AssetManager.pch.h"

#include "../GraphAsset.h"
#include <Editor/Editor/Windows/EditorWindows/ChainGraph/GraphEditorBase.h>
#include <Editor/Editor/Windows/EditorWindows/ChainGraph/GraphTool.h>
#include <Tools/ImGui/MuninGraph/MuninScriptGraph.h>

GraphAsset::GraphAsset(const std::filesystem::path &aFilePath) : AssetBase(aFilePath)
{
}

void GraphAsset::Init()
{

    if (!std::filesystem::exists(AssetPath))
    {
        Logger::Warn("GraphAsset does not exist: " + AssetPath.string());
        return;
    }

    std::ifstream file(AssetPath.string());
    const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();

    ScriptGraphSchema::DeserializeScriptGraph(graphToEdit, inGraph);

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

    auto ScriptEditor = Graph::GraphTool::Get().OpenScriptingEditor(AssetPath);

    if (!ScriptEditor->IsUsingSame(graphToEdit))
    {
        ScriptEditor->Init(graphToEdit, AssetPath);
    }

    ScriptEditor->Update(Timer::GetDeltaTime());
    ScriptEditor->Render(AssetPath.stem().string());
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
