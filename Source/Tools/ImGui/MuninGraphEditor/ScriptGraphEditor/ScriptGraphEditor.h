#pragma once

#include <DirectX/Shipyard/Texture.h>
#include <Engine/AssetManager/Objects/BaseAssets/GraphAsset.h>
#include <Tools/ImGui/MuninGraphEditor/GraphEditor/GraphEditorBase.h>
#include <Tools/ImGui/MuninGraphEditor/ScriptGraphEditor/ScriptGraphEditorTypes.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

class ScriptGraphSchema;
class ScriptGraph;
class ScriptGraphNode;
struct ScriptGraphEdge;
class ScriptGraphPin;

struct ScriptGraphEditorSettings : public GraphEditorSettingsBase
{
    std::shared_ptr<Texture> NodeHeaderTexture;
    std::shared_ptr<Texture> NodeGradientTexture;

    struct NodeTypeSettings
    {
        unsigned Type;
        std::shared_ptr<Texture> Icon;
        GraphColor Color;
        // Only has an effect on Variable nodes.
        bool UseTypeColor = false;
        bool CenterTitle = false;
    };

    std::unordered_map<unsigned, NodeTypeSettings> NodeSettings;

    ScriptGraphEditorSettings();
};

struct ScriptGraphEditorState : public GraphEditorStateBase
{
    bool ShowFlow = false;

    struct VariableEditorState
    {
        int CurrentVariable = -1;
        std::string VarToDelete;
        std::string NewVarName;
        int NewVarTypeIndex = -1;
        TypedDataContainer NewVariableDefault;
    } VariableEditor;

    struct GraphErrorState
    {
        bool HasError = false;
        std::string Message;
        size_t Node;
    } GraphError;

    GraphEditorContextMenuCategory VariablesContextCategory;

    std::vector<std::string> VisibleEditorTypes;
};

class ScriptGraphEditor : public GraphEditorBase<ScriptGraphNode, ScriptGraphEdge, ScriptGraphPin, ScriptGraphSchema>
{

    friend class GraphAsset;

  public:
    ScriptGraphEditor(ScriptGraphEditorSettings *aSettings, ScriptGraphEditorState *aState,
                      std::shared_ptr<GraphAsset> aGraph);

    std::vector<uint8_t> TEMP_SAVE_LOAD_dataBlock;

  protected:
    void RenderEditorStyle() override;
    void RenderNode(const std::shared_ptr<ScriptGraphNode> &aNode) override;
    void RenderNodeHeader(const std::shared_ptr<ScriptGraphNode> &aNode, const ImVec2 &aNodeHeaderRect) override;
    void RenderNodeStyle(const std::shared_ptr<ScriptGraphNode> &aNode, const ImRect &aNodeHeaderRect,
                         const ImRect &aNodeBodyRect) override;
    void RenderPin(const ScriptGraphPin &aPin) override;
    void RenderPinIcon(const ScriptGraphPin &aPin, const ImRect &aPinRect) override;
    void RenderEdge(const ScriptGraphEdge &anEdge) override;
    void RenderToolbar() override;
    void RenderUserBackgroundContextMenu() override;

    void HandleUserTransaction() override;

    std::string_view GetEditorTitle() const override;
    std::string_view GetEditorToken() const override;

    std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> HandleContextMenuSearch(
        std::string_view aSearchQuery) override;
    void HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem &aItem) override;

    void ReportError(const ScriptGraph *aGraph, size_t aNodeId, std::string_view aErrorMessage);

  private:
    void HandleCopyPaste() override;
    void UpdateVariablesContextMenu() const;
    void ScriptGraphEditor_TriggerEntryPointDialog() const;
    void ScriptGraphEditor_EditVariablesDialog() const;
};
