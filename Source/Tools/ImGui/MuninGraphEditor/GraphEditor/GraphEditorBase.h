#pragma once
#include <algorithm>
#include <fstream>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "GraphEditorUtils.h"
#include "Tools/ImGui/ImGui/Font/IconsFontAwesome5.h"
#include "Tools/ImGui/ImGui/imgui_node_editor.h"
#include "Tools/ImGui/ImGui/imgui_node_editor_internal.h"
#include "Tools/ImGui/ImGui/misc/cpp/imgui_stdlib.h"
#include "Tools/ImGui/MuninGraph/MuninGraph.h"
#include "Tools/ImGui/MuninGraph/NodeGraph/NodeGraphCommon.h"
#include <Engine/AssetManager/Objects/BaseAssets/GraphAsset.h>

namespace ImNodeEd = ax::NodeEditor;

struct GraphEditorSettingsBase
{
    ImFont *NodeTitleFont = nullptr;
    ImFont *NodeBodyFont = nullptr;
    ImFont *EditorTokenFont = nullptr;
    ImFont *EditorFont = nullptr;

    ImFont *FontAwesomeRegular = nullptr;
    ImFont *FontAwesomeSolid = nullptr;

    GraphEditorSettingsBase();
    virtual ~GraphEditorSettingsBase();
};

struct GraphEditorContextMenuItem
{
    std::string Title;
    const RegisteredNodeClass *Value;
    std::string Tag;
};

struct GraphEditorContextMenuCategory
{
    std::string Title;
    std::vector<GraphEditorContextMenuItem> Items;
};

struct GraphEditorStateBase
{
    virtual ~GraphEditorStateBase();

    struct BackgroundContextMenuInfo
    {
        std::unordered_map<std::string, GraphEditorContextMenuCategory> Categories;
        std::vector<std::string> CategoryNamesList;
    } BackgroundContextMenu;

    struct ContextSearchInfo
    {
        bool FieldChanged = false;
        bool SetKeyboardFocus = false;
        std::string Value;
        struct SearchMenuItem
        {
            GraphEditorContextMenuItem Item;
            float Rank = FLT_MAX;
        };
        std::vector<SearchMenuItem> SearchResults;
    } ContextSearch;

    struct InternalInfo
    {
        bool RefreshNodePositions = true;
    } Layout;

    size_t PinContextMenuId = 0;
};

template <typename GraphNodeClass, typename GraphEdgeClass, typename GraphPinClass, typename GraphSchemaClass>
class GraphEditorBase
{
  public:
    GraphEditorBase(GraphEditorSettingsBase *aSettings, GraphEditorStateBase *aState,
                    std::shared_ptr<GraphAsset> aGraph)
        : mySettings(aSettings), myEditorState(aState)
    {
        // This is very buggy so turning it off.
        myNodeEditorConfig.SettingsFile = nullptr;
        myNodeEditorConfig.CanvasSizeMode = ax::NodeEditor::CanvasSizeMode::CenterOnly;

        myEditorContext = ImNodeEd::CreateEditor(&myNodeEditorConfig);
        aGraphAsset = aGraph;
        myGraph = aGraph->GetGraph().get();
        mySchema = new GraphSchemaClass(myGraph);

        const std::vector<const RegisteredNodeClass *> schemaNodeClasses =
            MuninGraph::Get().GetSupportedNodeClasses(typeid(GraphSchemaClass));
        for (const auto nodeClass : schemaNodeClasses)
        {
            if (nodeClass->HasFlag(CF_Internal))
                continue;

            if (nodeClass->ClassDefaultObject)
                if (auto existingCat = myEditorState->BackgroundContextMenu.Categories.find(nodeClass->Category);
                    existingCat != myEditorState->BackgroundContextMenu.Categories.end())
                {
                    GraphEditorContextMenuCategory &existingCategory = existingCat->second;
                    GraphEditorContextMenuItem newItem;
                    newItem.Title = nodeClass->Name;
                    newItem.Value = nodeClass;
                    existingCategory.Items.emplace_back(newItem);
                }
                else
                {
                    GraphEditorContextMenuCategory newCategory;
                    newCategory.Title = nodeClass->Category;
                    GraphEditorContextMenuItem newItem;
                    newItem.Title = nodeClass->Name;
                    newItem.Value = nodeClass;
                    newCategory.Items.emplace_back(newItem);
                    myEditorState->BackgroundContextMenu.Categories.emplace(nodeClass->Category, newCategory);
                    myEditorState->BackgroundContextMenu.CategoryNamesList.push_back(nodeClass->Category);
                }
        }

        std::ranges::sort(myEditorState->BackgroundContextMenu.CategoryNamesList);
    }

    virtual ~GraphEditorBase()
    {
        if (myEditorContext)
        {
            ImNodeEd::DestroyEditor(myEditorContext);
        }
        myEditorState = nullptr;
        mySettings = nullptr;
    }

    // virtual void Init(GraphClass* aGraph);
    virtual void Render();

  protected:
    typedef GraphEditorBase Base;
    ScriptGraph *myGraph = nullptr;
    std::shared_ptr<GraphAsset> aGraphAsset = nullptr;
    GraphSchemaClass *mySchema = nullptr;
    ImNodeEd::EditorContext *myEditorContext = nullptr;
    ImNodeEd::Config myNodeEditorConfig;
    GraphEditorSettingsBase *mySettings;
    GraphEditorStateBase *myEditorState;

    float JaroWinklerDistance(std::string_view s1, std::string_view s2, bool caseSensitive = false)
    {
        float m = 0;
        size_t low, high;
        size_t k = 0, t = 0;

        if (s1.empty() || s2.empty())
            return 0;

        std::string str1(s1);
        std::string str2(s2);

        if (!caseSensitive)
        {
            std::ranges::transform(str1, str1.begin(), [](unsigned char c) { return (unsigned char)std::tolower(c); });
            std::ranges::transform(str2, str2.begin(), [](unsigned char c) { return (unsigned char)std::tolower(c); });
        }

        if (str1 == str2)
            return 1;

        const size_t range = (std::max)(s1.length(), s2.length()) / 2 - 1;

        std::vector<size_t> str1Matches;
        str1Matches.resize(str1.length());
        std::vector<size_t> str2Matches;
        str2Matches.resize(str2.length());

        for (size_t i = 0; i < str1.length(); i++)
        {
            if (i >= range)
            {
                low = i - range;
            }
            else
            {
                low = 0;
            }

            if (i + range <= str2.length() - 1)
            {
                high = i + range;
            }
            else
            {
                high = s2.length() - 1;
            }

            for (size_t j = low; j <= high; j++)
            {
                if (str1Matches[i] != 1 && str2Matches[j] != 1 && str1[i] == str2[j])
                {
                    m += 1;
                    str1Matches[i] = 1;
                    str2Matches[j] = 1;
                    break;
                }
            }
        }

        if (m == 0)
        {
            return 0;
        }

        for (size_t i = 0; i < str1.length(); i++)
        {
            if (str1Matches[i] == 1)
            {
                size_t j = 0;
                for (j = k; j < str2.length(); j++)
                {
                    if (str2Matches[j] == 1)
                    {
                        k = j + 1;
                        break;
                    }
                }

                if (str1[i] != str2[j])
                {
                    t += 1;
                }
            }
        }

        float w = (m / str1.length() + m / str2.length() + (m - t / 2) / m) / 3;
        if (w > 0.7f)
        {
            float p = 0.1f;
            size_t l = 0;
            while (str1[l] == str2[l] && l < 4)
            {
                l += 1;
            }

            w += l * p * (1 - w);
        }

        return w;
    }

    //~ Render Functions
    // This is responsible for rendering the actual window we live in.
    // It calls all the other rendering functions to render each specific part.
    virtual void RenderWindow();

    // If you have a Toolbar in your window you render it here.
    // By default it renders Save/Load buttons.
    virtual void RenderToolbar();
    virtual void RenderBackgroundContextMenu();
    virtual void RenderUserBackgroundContextMenu();
    virtual void RenderPinContextMenu();
    virtual void RenderUserPinContextMenu();

    virtual void RenderEditorStyle();

    virtual void RenderNode(const std::shared_ptr<GraphNodeClass> &aNode);
    virtual void RenderNodeHeader(const std::shared_ptr<GraphNodeClass> &aNode, const ImVec2 &aNodeHeaderRect);
    virtual void RenderNodeStyle(const std::shared_ptr<GraphNodeClass> &aNode, const ImRect &aNodeHeaderRect,
                                 const ImRect &aNodeBodyRect);

    virtual void RenderPin(const GraphPinClass &aPin);
    virtual void RenderPinIcon(const GraphPinClass &aPin, const ImRect &aPinRect);

    virtual void RenderEdge(const GraphEdgeClass &anEdge);
    virtual void RenderGraphEditor();
    //~

    virtual void HandleImNodeCreateNode();
    virtual void HandleImNodeDeleteNode();
    virtual void HandleUserTransaction();
    virtual std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> HandleContextMenuSearch(
        std::string_view aSearchQuery);
    virtual void HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem &aItem);

    virtual std::string_view GetEditorTitle() const;
    virtual std::string_view GetEditorToken() const;
};

// Helps improve readability of this file :P
#define GraphEditorMethod(...)                                                                                         \
    template <typename GraphNodeClass, typename GraphEdgeClass, typename GraphPinClass, typename GraphSchemaClass>     \
    __VA_ARGS__ GraphEditorBase<GraphNodeClass, GraphEdgeClass, GraphPinClass, GraphSchemaClass>

GraphEditorMethod(void)::Render()
{
    ImGui::PushFont(mySettings->EditorFont);
    RenderWindow();
    ImGui::PopFont();
}

GraphEditorMethod(void)::HandleImNodeCreateNode()
{
    if (ImNodeEd::BeginCreate())
    {
        ImNodeEd::PinId startLinkId, endLinkId;

        // This returns True constantly while trying to create a link, even before we've selected an end pin.
        if (ImNodeEd::QueryNewLink(&startLinkId, &endLinkId))
        {
            if (startLinkId && endLinkId)
            {

                std::string canCreateEdgeFeedback;
                if (!mySchema->CanCreateEdge(startLinkId.Get(), endLinkId.Get(), canCreateEdgeFeedback))
                {
                    ImNodeEd::RejectNewItem(ImColor(255, 0, 0, 255));
                }

                // This is true if we've made a link between two pins. I.e. when we release LMB to make a link.
                if (ImNodeEd::AcceptNewItem())
                {
                    mySchema->CreateEdge(startLinkId.Get(), endLinkId.Get());
                }
            }
        }
    }
    ImNodeEd::EndCreate();
}

GraphEditorMethod(void)::HandleImNodeDeleteNode()
{
    if (ImNodeEd::BeginDelete())
    {
        ImNodeEd::LinkId deletedLinkId;
        while (ImNodeEd::QueryDeletedLink(&deletedLinkId))
        {
            if (ImNodeEd::AcceptDeletedItem())
            {
                mySchema->RemoveEdge(deletedLinkId.Get());
            }
        }

        ImNodeEd::NodeId deletedNodeId = 0;
        while (ImNodeEd::QueryDeletedNode(&deletedNodeId))
        {
            if (ImNodeEd::AcceptDeletedItem())
            {
                mySchema->RemoveNode(deletedNodeId.Get());
            }
        }
    }
    ImNodeEd::EndDelete();
}

GraphEditorMethod(void)::HandleUserTransaction()
{
}

GraphEditorMethod(std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem>)::HandleContextMenuSearch(
    std::string_view aSearchQuery)
{
    std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> results;
    const std::vector<const RegisteredNodeClass *> schemaNodeClasses =
        MuninGraph::Get().GetSupportedNodeClasses(typeid(GraphSchemaClass));
    for (const auto &nodeClass : schemaNodeClasses)
    {
        if (nodeClass->HasFlag(CF_Internal))
            continue;

        const float rank = JaroWinklerDistance(nodeClass->Name, aSearchQuery);
        GraphEditorContextMenuItem foundItem;
        foundItem.Title = nodeClass->Name;
        foundItem.Value = nodeClass;
        results.emplace_back(foundItem, rank);
    }

    std::ranges::sort(results,
                      [](const GraphEditorStateBase::ContextSearchInfo::SearchMenuItem &A,
                         const GraphEditorStateBase::ContextSearchInfo::SearchMenuItem &B) { return A.Rank > B.Rank; });

    return results;
}

GraphEditorMethod(void)::HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem &aItem)
{
    const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
    if (const std::shared_ptr<GraphNodeClass> newNode = mySchema->CreateNode(*aItem.Value))
    {
        auto uidNewNode = AsObjectUIDSharedPtr(newNode);
        ImNodeEd::SetNodePosition(uidNewNode->GetUID(), mousePos);
    }
}

GraphEditorMethod(std::string_view)::GetEditorTitle() const
{
    return "MuninGraph Editor";
}

GraphEditorMethod(std::string_view)::GetEditorToken() const
{
    return "Graph";
}

GraphEditorMethod(void)::RenderGraphEditor()
{
    ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_SnapLinkToPinDir, 1.0f);
    ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 100.0f);

    for (const auto &[id, node] : mySchema->GetNodes())
    {
        if (myEditorState->Layout.RefreshNodePositions)
        {
            float X, Y, Z;
            node->GetNodePosition(X, Y, Z);
            ImNodeEd::SetNodePosition(id, {X, Y});
            ImNodeEd::SetNodeZPosition(id, Z);
        }

        RenderNode(node);

        // Sync node position with ImNodeEd node position in case someone moved it in editor.
        ImVec2 newNodePos = ImNodeEd::GetNodePosition(id);
        float newNodeZ = ImNodeEd::GetNodeZPosition(id);
        node->SetNodePosition(newNodePos.x, newNodePos.y, newNodeZ);
    }

    myEditorState->Layout.RefreshNodePositions = false;

    for (const auto &[id, edge] : mySchema->GetEdges())
    {
        RenderEdge(edge);
    }

    ImNodeEd::PopStyleVar(2);
}

GraphEditorMethod(void)::RenderWindow()
{
    ImNodeEd::SetCurrentEditor(myEditorContext);
    ImGui::SetNextWindowSize({1280, 720}, ImGuiCond_FirstUseEver);
    if (ImGui::BeginChild(GetEditorTitle().data()))
    {
        RenderToolbar();
        ImNodeEd::Begin("MuninGraphEditor");

        ImNodeEd::Suspend();
        ImVec2 tokenPos = ImGui::GetWindowContentRegionMax();
        ImGui::PushFont(mySettings->EditorTokenFont);
        const ImVec2 tokenSize = ImGui::CalcTextSize(GetEditorToken().data());
        tokenPos = {tokenPos.x - tokenSize.x - 24, tokenPos.y - tokenSize.y - 12};

        ImGui::SetCursorPos(tokenPos);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 128));
        ImGui::TextUnformatted(GetEditorToken().data());
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImNodeEd::Resume();
        RenderGraphEditor();

        HandleImNodeCreateNode();
        HandleImNodeDeleteNode();
        HandleUserTransaction();

        ImNodeEd::Suspend();

        if (ImNodeEd::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("BackgroundContextMenu");
        }

        RenderBackgroundContextMenu();

        for (auto &pinId : mySchema->GetPins() | std::views::keys)
        {
            ImNodeEd::PinId id = pinId;
            // ShowPinContextMenu returns true for all pins on the same node
            // regardless of which one you actually clicked on. Therefore you
            // need to check if it's the pin that is being hovered that you
            // clicked on to get the correct context.
            if (ImNodeEd::GetHoveredPin() == id)
            {
                if (ImNodeEd::ShowPinContextMenu(&id))
                {
                    myEditorState->PinContextMenuId = pinId;
                    ImGui::OpenPopup("PinContextMenu");
                    break; // Only one popup please xP
                }
            }
        }

        RenderPinContextMenu();

        ImNodeEd::Resume();
        RenderEditorStyle();
        ImNodeEd::End();
    }
    ImGui::EndChild();
    ImNodeEd::SetCurrentEditor(nullptr);
}

GraphEditorMethod(void)::RenderToolbar()
{
    // No default toolbar to render.
}

GraphEditorMethod(void)::RenderBackgroundContextMenu()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("BackgroundContextMenu"))
    {
        myEditorState->ContextSearch.FieldChanged =
            ImGui::InputText("##ctxtMenuSearch", &myEditorState->ContextSearch.Value);
        if (myEditorState->ContextSearch.SetKeyboardFocus)
        {
            ImGui::SetKeyboardFocusHere(0);
            myEditorState->ContextSearch.SetKeyboardFocus = false;
        }

        if (myEditorState->ContextSearch.Value.empty())
        {
            for (const std::string &categoryName : myEditorState->BackgroundContextMenu.CategoryNamesList)
            {
                const auto &category = myEditorState->BackgroundContextMenu.Categories.at(categoryName);
                if (ImGui::TreeNodeEx(category.Title.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    for (const auto &item : category.Items)
                    {
                        if (ImGui::TreeNodeEx(item.Title.c_str(),
                                              ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
                        {
                            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                            {
                                HandleBackgroundContextMenuItemClicked(item);
                                myEditorState->ContextSearch.Value.clear();
                                myEditorState->ContextSearch.FieldChanged = false;
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }

            RenderUserBackgroundContextMenu();
        }
        else
        {
            if (myEditorState->ContextSearch.FieldChanged)
            {
                myEditorState->ContextSearch.SearchResults.clear();
                myEditorState->ContextSearch.SearchResults =
                    std::move(HandleContextMenuSearch(myEditorState->ContextSearch.Value));
            }

            for (const auto &item : myEditorState->ContextSearch.SearchResults)
            {
                if (ImGui::TreeNodeEx(item.Item.Title.c_str(),
                                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                    {
                        HandleBackgroundContextMenuItemClicked(item.Item);
                        myEditorState->ContextSearch.Value.clear();
                        myEditorState->ContextSearch.FieldChanged = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

GraphEditorMethod(void)::RenderUserBackgroundContextMenu()
{
    // Nothing by default. Can override this to extend the menu with your own items.
}

GraphEditorMethod(void)::RenderPinContextMenu()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("PinContextMenu"))
    {
        const ScriptGraphPin *pin = mySchema->GetPin(myEditorState->PinContextMenuId);
        ImGui::BeginDisabled(!pin->IsDynamicPin());
        if (ImGui::TreeNodeEx("Delete Pin", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mySchema->MarkDynamicPinForDelete(myEditorState->PinContextMenuId);
                myEditorState->PinContextMenuId = 0;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::TreePop();
        ImGui::EndDisabled();
        ImGui::EndPopup();
    }
    else
    {
        myEditorState->PinContextMenuId = 0;
    }
    ImGui::PopStyleVar();
}

GraphEditorMethod(void)::RenderUserPinContextMenu()
{
    // Nothing by default. Can override this to extend the menu with your own items.
}

GraphEditorMethod(void)::RenderEditorStyle()
{
    // Nothing by default.
}

GraphEditorMethod(void)::RenderNode(const std::shared_ptr<GraphNodeClass> &aNode)
{
    std::vector<const GraphPinClass *> inputPins;
    std::vector<const GraphPinClass *> outputPins;

    ImGui::PushFont(mySettings->NodeTitleFont);
    const ImVec2 headerTextSize = ImGui::CalcTextSize(aNode->GetNodeTitle().data());
    ImGui::PopFont();

    ImVec2 leftMinSize = {64, 0};
    ImVec2 rightMinSize = {64, 0};

    ImGui::PushFont(mySettings->NodeBodyFont);
    for (const auto &[id, pin] : aNode->GetPins())
    {
        const ImVec2 currentTextSize = ImGui::CalcTextSize(pin.GetLabel().data());

        if (pin.GetPinDirection() == PinDirection::Input)
        {
            if (currentTextSize.x > leftMinSize.x)
                leftMinSize.x = currentTextSize.x;
            if (currentTextSize.y > leftMinSize.y)
                leftMinSize.y = currentTextSize.y;

            inputPins.push_back(&pin);
        }
        else
        {
            if (currentTextSize.x > rightMinSize.x)
                rightMinSize.x = currentTextSize.x;
            if (currentTextSize.y > rightMinSize.y)
                rightMinSize.y = currentTextSize.y;

            outputPins.push_back(&pin);
        }
    }
    ImGui::PopFont();

    if (leftMinSize.x > 0)
        leftMinSize.x += 48;

    if (rightMinSize.x > 0)
        rightMinSize.x += 48;

    std::sort(inputPins.begin(), inputPins.end(),
              [](const GraphPinClass *A, const GraphPinClass *B) { return A->GetUID() < B->GetUID(); });
    std::sort(outputPins.begin(), outputPins.end(),
              [](const GraphPinClass *A, const GraphPinClass *B) { return A->GetUID() < B->GetUID(); });

    ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeRounding, 3.0f);
    ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

    const auto uidAwareNode = AsObjectUIDSharedPtr(aNode);
    const ImNodeEd::NodeId currentImNodeId = uidAwareNode->GetUID();
    ImNodeEd::BeginNode(currentImNodeId);
    ImGui::PushID(static_cast<int>(uidAwareNode->GetUID()));

    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos({cursorPos.x, cursorPos.y + 2});

    const float bodyMinWidth = leftMinSize.x + rightMinSize.x + 16;
    const ImVec2 nodePinTableSize = {headerTextSize.x > bodyMinWidth ? headerTextSize.x : bodyMinWidth, 0};

    // Node Header
    ImGui::BeginTable("nodeHeader", 2, ImGuiTableFlags_SizingFixedFit, nodePinTableSize);
    ImGui::TableNextColumn();
    RenderNodeHeader(aNode, nodePinTableSize);
    ImGui::EndTable();

    const ImVec2 headerRectMin = ImGui::GetItemRectMin();
    const ImVec2 headerRectMax = ImGui::GetItemRectMax();
    const ImRect nodeHeaderRect(headerRectMin, headerRectMax);

    // Node Pins
    ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp, nodePinTableSize);
    ImGui::TableNextRow();
    ImGui::PushFont(mySettings->NodeBodyFont);
    const size_t numRows = inputPins.size() > outputPins.size() ? inputPins.size() : outputPins.size();
    for (size_t row = 0; row < numRows; row++)
    {
        ImGui::TableNextColumn();
        if (row < inputPins.size())
        {
            const GraphPinClass *inputPin = inputPins[row];
            RenderPin(*inputPin);
        }
        ImGui::TableNextColumn();
        if (row < outputPins.size())
        {
            const GraphPinClass *outputPin = outputPins[row];
            RenderPin(*outputPin);
        }
    }
    ImGui::PopFont();
    ImGui::EndTable();

    const ImVec2 nodeBodyMin = ImGui::GetItemRectMin();
    const ImVec2 nodeBodyMax = ImGui::GetItemRectMax();
    const ImRect nodeBodyRect(nodeBodyMin, nodeBodyMax);
    ImNodeEd::EndNode();

    // This cannot happen before ImNodeEd::EndNode() since the DrawList won't be available.
    RenderNodeStyle(aNode, nodeHeaderRect, nodeBodyRect);

    ImGui::PopID();

    ImNodeEd::PopStyleVar();
    ImNodeEd::PopStyleVar();
}

GraphEditorMethod(void)::RenderNodeHeader(const std::shared_ptr<GraphNodeClass> &aNode, const ImVec2 &aNodeHeaderRect)
{
    aNodeHeaderRect;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2.0f);
    ImGui::PushFont(mySettings->NodeTitleFont);
    ImGui::TextUnformatted(aNode->GetNodeTitle().data());
    ImGui::PopFont();
}

GraphEditorMethod(void)::RenderNodeStyle(const std::shared_ptr<GraphNodeClass> &aNode, const ImRect &aNodeHeaderRect,
                                         const ImRect &aNodeBodyRect)
{
    // No styling by default.
    aNode;
    aNodeHeaderRect;
    aNodeBodyRect;
}

GraphEditorMethod(void)::RenderPin(const GraphPinClass &aPin)
{
    const PinDirection pinDir = aPin.GetPinDirection();
    const float iconSize = 24.0f;
    const ImVec2 iconSizeRect(iconSize, iconSize);

    if (pinDir == PinDirection::Input)
    {
        ImNodeEd::BeginPin(aPin.GetUID(), static_cast<ImNodeEd::PinKind>(pinDir));

        const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        const ImRect sizeRect(cursorPos, cursorPos + iconSizeRect);

        if (ImGui::IsRectVisible(iconSizeRect))
        {
            RenderPinIcon(aPin, sizeRect);
        }

        ImGui::Dummy(iconSizeRect);

        ImNodeEd::EndPin();
        ImGui::SameLine();

        const float y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + 3);
        ImGui::TextUnformatted(aPin.GetLabel().data());
        ImGui::SetCursorPosY(y);
    }
    else // Output
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
                             ImGui::CalcTextSize(aPin.GetLabel().data()).x - ImGui::GetScrollX() -
                             2 * ImGui::GetStyle().ItemSpacing.x - 14.0f);

        const float y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + 3);
        ImGui::TextUnformatted(aPin.GetLabel().data());
        ImGui::SameLine();
        ImGui::SetCursorPosY(y);

        ImNodeEd::BeginPin(aPin.GetUID(), static_cast<ImNodeEd::PinKind>(pinDir));

        const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        const ImRect sizeRect(cursorPos, cursorPos + iconSizeRect);

        if (ImGui::IsRectVisible(iconSizeRect))
        {
            RenderPinIcon(aPin, sizeRect);
        }

        ImGui::Dummy(iconSizeRect);
        ImNodeEd::EndPin();
    }
}

GraphEditorMethod(void)::RenderPinIcon(const GraphPinClass &aPin, const ImRect &aPinRect)
{
    const GraphColor emptyColor(32, 32, 32, 255);
    const bool pinConnected = aPin.IsConnected();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    GraphEditorUtils::DrawIcon(drawList, aPinRect, GraphIcon::Circle, pinConnected, GraphColor::White, emptyColor);
}

GraphEditorMethod(void)::RenderEdge(const GraphEdgeClass &anEdge)
{
    const GraphColor White = GraphColor::White.AsNormalized();
    ImNodeEd::Link(anEdge.EdgeId, anEdge.FromId, anEdge.ToId, {White.R, White.G, White.B, White.A}, 1.0f);
}

#undef GraphEditorMethod
