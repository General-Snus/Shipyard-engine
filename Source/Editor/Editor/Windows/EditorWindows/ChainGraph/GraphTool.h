#pragma once
#include <Tools/ImGui/MuninGraphEditor/ScriptGraphEditor/ScriptGraphEditor.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
namespace Graph
{
class GraphTool
{
  public:
    friend class Singleton<GraphTool>;

    static std::shared_ptr<ScriptGraphEditor> OpenScriptingEditor(const std::filesystem::path &AssetPath);
    static void RunEditor();
    static void Init();

    static inline std::string CopiedNodes; 
  private:
    static inline std::unordered_map<std::filesystem::path, std::shared_ptr<ScriptGraphEditor>> myScriptEditors;
    static inline ScriptGraphEditorSettings *setting;
    static inline ScriptGraphEditorState *state; 
};
} // namespace Graph
