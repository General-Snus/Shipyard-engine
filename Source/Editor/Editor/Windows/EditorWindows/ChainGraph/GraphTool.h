#pragma once
#include "ScriptGraphEditor/ScriptGraphEditor.h"
#include <Tools/Utilities/System/SingletonTemplate.h>
namespace Graph
{
class GraphTool : public Singleton<GraphTool>
{
  public:
    friend class Singleton<GraphTool>;

    std::shared_ptr<ScriptGraphEditor> OpenScriptingEditor(const std::filesystem::path &AssetPath);

  private:
    std::unordered_map<std::filesystem::path, std::shared_ptr<ScriptGraphEditor>> myScriptEditors;
    GraphTool();
    ~GraphTool();
};
} // namespace Graph
