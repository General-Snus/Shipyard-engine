#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Graph/GlobalUID.h>
#include <Graph/GraphColor.h>
#include <Graph/GraphTypes.h>
#include <imgui_node_editor.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ImNodeEd = ax::NodeEditor;

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
class GraphEditorBase
{
	// These should be moved to a class that manages these and
	// also holds a Device and Device Context pointer for DX
	// so we don't depend on RHI for loading memory textures.
	static inline ImFont* ourNodeTitleFont = nullptr;
	static inline ImFont* ourNodeBodyFont = nullptr;

	static inline ImFont* ourEditorTokenFont = nullptr;

	std::string myEditorToken;

protected:

	typedef GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType> Super;

	struct SearchMenuItem
	{
		std::string Title;
		std::string Value;
		size_t Rank = SIZE_MAX;
		std::string Tag;
	};

	struct ContextMenuItem
	{
		std::string Title;
		std::string Value;
		std::string Tag;
	};

	struct ContextMenuCategory
	{
		std::string Title;
		std::vector<ContextMenuItem> Items;
	};

	struct EditorState
	{
		struct SearchState
		{
			std::string bgCtxtSearchField;
			bool bgCtxtSearchFieldChanged = false;
			bool bgCtxtSearchFocus = false;
			std::vector<SearchMenuItem> bgCtxtSearchFieldResults;
		} Search;

		bool showFlow = false;

		bool initNavToContent = false;

		std::string editorTitle = "Graph Editor";
	} myEditorState;

	// Actual storage of looking up categories when generating.
	static inline std::unordered_map<std::string,ContextMenuCategory> myBgContextCategories;
	// Sorted list of context category names.
	static inline std::vector<std::string> myBgContextCategoryNamesList;

	std::shared_ptr<GraphType> myGraph;
	std::unique_ptr<SchemaType> mySchema;

	ImNodeEd::EditorContext* myEditorContext = nullptr;

	template<typename T>
	std::enable_if_t<std::is_same_v<T,std::string> || std::is_same_v<T,std::wstring>,typename T::size_type>
		LevenshteinDistance(const T& aString,const T& aSearchString)
	{
		if (aString.size() > aSearchString.size())
		{
			return LevenshteinDistance(aSearchString,aString);
		}

		using TSize = typename T::size_type;
		const TSize minSize = aString.size();
		const TSize maxSize = aSearchString.size();
		std::vector<TSize> levenDistance(minSize + 1);

		for (TSize s = 0; s <= minSize; ++s)
		{
			levenDistance[s] = s;
		}

		for (TSize s = 1; s <= maxSize; ++s)
		{
			TSize lastDiag = levenDistance[0],lastDiagMem;
			++levenDistance[0];

			for (TSize t = 1; t <= minSize; ++t)
			{
				lastDiagMem = levenDistance[t];
				if (aString[t - 1] == aSearchString[s - 1])
				{
					levenDistance[t] = lastDiag;
				}
				else
				{
					levenDistance[t] = std::min(std::min(levenDistance[t - 1],levenDistance[t]),lastDiag) + 1;
				}
				lastDiag = lastDiagMem;
			}
		}

		return levenDistance[minSize];
	}

	virtual void HandleImNodeCreateNode();
	virtual void HandleImNodeDeleteNode();

	virtual std::shared_ptr<GraphNodeType> CreateNode(const typename GraphType::SupportedNodeClass& aNodeClass);

	virtual void RenderWindow();
	virtual void RenderGraphEditor();

	virtual void RenderNode(const std::shared_ptr<GraphNodeType>& aNode) = 0;
	virtual void RenderEdge(const GraphEdgeType& anEdge);

	virtual void RenderBackgroundContextMenu();

public:

	virtual ~GraphEditorBase();

	virtual void Init(std::shared_ptr<GraphType> aGraph);
	void Render();
};

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::HandleImNodeCreateNode()
{
	if (ImNodeEd::BeginCreate())
	{
		ImNodeEd::PinId startLinkId,endLinkId;

		// This returns True constantly while trying to create a link, even before we've selected an end pin.
		if (ImNodeEd::QueryNewLink(&startLinkId,&endLinkId))
		{
			if (startLinkId && endLinkId)
			{

				std::string canCreateEdgeFeedback;
				if (!mySchema->CanCreateEdge(startLinkId.Get(),endLinkId.Get(),canCreateEdgeFeedback))
				{
					ImNodeEd::RejectNewItem(ImColor(255,0,0,255));
				}

				// This is true if we've made a link between two pins. I.e. when we release LMB to make a link.
				if (ImNodeEd::AcceptNewItem())
				{
					mySchema->CreateEdge(startLinkId.Get(),endLinkId.Get());
				}
			}
		}
	}
	ImNodeEd::EndCreate();
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::HandleImNodeDeleteNode()
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

template <typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename
	SchemaType>
std::shared_ptr<GraphNodeType> GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::CreateNode(
	const typename GraphType::SupportedNodeClass& aNodeClass)
{
	return mySchema->AddNode(aNodeClass);
}

template <typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename
	SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::RenderWindow()
{
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::RenderEdge(const GraphEdgeType& anEdge)
{
	const GraphColor typeColor = anEdge.Color.AsNormalized();
	ImNodeEd::Link(anEdge.EdgeId,anEdge.FromUID,anEdge.ToUID,{ typeColor.R, typeColor.G, typeColor.B, typeColor.A },anEdge.Thickness);
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::RenderGraphEditor()
{
	ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_SnapLinkToPinDir,1.0f);
	ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength,0.0f);
	for (const auto& [nodeUID,node] : myGraph->GetNodes())
	{
		RenderNode(node);
		// Cache node positions, sigh.
		// NOTE: This just happens because we can't read node positions
		// from ImNodeEd if we don't have an active ImGui window with
		// that node editor, buh.
		const ImVec2 nodePos = ImNodeEd::GetNodePosition(nodeUID);
		const float nodeZ = ImNodeEd::GetNodeZPosition(nodeUID);
		node->UpdateNodePositionCache(nodePos.x,nodePos.y,nodeZ);
	}


	for (const auto& [edgeId,edge] : myGraph->GetEdges())
	{
		RenderEdge(edge);
	}

	ImNodeEd::PopStyleVar(2);

	HandleImNodeCreateNode();
	HandleImNodeDeleteNode();

	ImNodeEd::Suspend();

	if (ImNodeEd::ShowBackgroundContextMenu())
	{
		myEditorState.Search.bgCtxtSearchFocus = true;
		ImGui::OpenPopup("BackgroundContextMenu");
	}
	else if (ImNodeEd::NodeId nodeId; ImNodeEd::ShowNodeContextMenu(&nodeId))
	{
		ImGui::OpenPopup("NodeContextMenu");
	}

	RenderBackgroundContextMenu();

	ImNodeEd::Resume();

	if (myEditorState.initNavToContent)
	{
		ImNodeEd::NavigateToContent();
		myEditorState.initNavToContent = false;
	}
}

template <typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename
	SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::RenderBackgroundContextMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(8,8));
	if (ImGui::BeginPopup("BackgroundContextMenu"))
	{
		//ImGui::PushItemWidth(100.0f);
		myEditorState.Search.bgCtxtSearchFieldChanged = ImGui::InputText("##nodeSearch",&myEditorState.Search.bgCtxtSearchField);
		if (myEditorState.Search.bgCtxtSearchFocus)
		{
			ImGui::SetKeyboardFocusHere(0);
			myEditorState.Search.bgCtxtSearchFocus = false;
		}
		//ImGui::PopItemWidth();

		if (myEditorState.Search.bgCtxtSearchField.empty())
		{
			for (const std::string& catName : myBgContextCategoryNamesList)
			{
				const auto catIt = myBgContextCategories.find(catName);
				if (ImGui::TreeNodeEx(catName.c_str(),ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					for (const auto& item : catIt->second.Items)
					{
						if (ImGui::TreeNodeEx(item.Title.c_str(),ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
						{
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							{
								const typename GraphType::SupportedNodeClass& type = GraphType::GetSupportedNodeTypes().find(item.Value)->second;
								std::shared_ptr<GraphNodeType> newNode;
								newNode = mySchema->AddNode(type);
								const auto uuidAwareNewNode = AsGUIDAwareSharedPtr(newNode);
								const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
								ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(),mousePos);
								ImGui::CloseCurrentPopup();
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
		}
		else
		{
			if (myEditorState.Search.bgCtxtSearchFieldChanged)
			{
				myEditorState.Search.bgCtxtSearchFieldResults.clear();
				const auto supportedNodeTypes = GraphType::GetSupportedNodeTypes();
				for (const auto& [nodeName,nodeType] : supportedNodeTypes)
				{
					myEditorState.Search.bgCtxtSearchFieldResults.push_back({
						nodeType.NodeTitle,
						nodeType.TypeName,
						0,
						""
						});

					myEditorState.Search.bgCtxtSearchFieldResults.back().Rank = LevenshteinDistance(myEditorState.Search.bgCtxtSearchFieldResults.back().Title,myEditorState.Search.bgCtxtSearchField);
				}

				std::sort(myEditorState.Search.bgCtxtSearchFieldResults.begin(),myEditorState.Search.bgCtxtSearchFieldResults.end(),
					[](const SearchMenuItem& A,const SearchMenuItem& B)
					{
						return A.Rank < B.Rank;
					});
			}

			for (auto& item : myEditorState.Search.bgCtxtSearchFieldResults)
			{
				if (ImGui::TreeNodeEx(item.Title.c_str(),ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						const typename GraphType::SupportedNodeClass& type = GraphType::GetSupportedNodeTypes().find(item.Value)->second;
						std::shared_ptr<GraphNodeType> newNode = CreateNode(type);

						const auto uuidAwareNewNode = AsGUIDAwareSharedPtr(newNode);
						const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
						ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(),mousePos);

						myEditorState.Search.bgCtxtSearchFieldChanged = false;
						myEditorState.Search.bgCtxtSearchField.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::~GraphEditorBase()
{
	if (myEditorContext)
	{
		ImNodeEd::DestroyEditor(myEditorContext);
	}
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::Init(std::shared_ptr<GraphType> aGraph)
{
	const auto aGraphUIDPtr = AsGUIDAwareSharedPtr(aGraph);
	myEditorToken = aGraphUIDPtr->GetTypeName();
	std::transform(myEditorToken.begin(),myEditorToken.end(),myEditorToken.begin(),toupper);

	const ImGuiIO& imGuiIO = ImGui::GetIO();
	if (!ourNodeTitleFont)
	{
		ourNodeTitleFont = imGuiIO.Fonts->AddFontFromFileTTF("Content/fonts/Roboto/Roboto-Bold.ttf",32);
	}

	if (!ourNodeBodyFont)
	{
		ourNodeBodyFont = imGuiIO.Fonts->AddFontFromFileTTF("Content/fonts/Roboto/Roboto-Regular.ttf",18);
	}

	if (!ourEditorTokenFont)
	{
		ourEditorTokenFont = imGuiIO.Fonts->AddFontFromFileTTF("Content/fonts/Roboto/Roboto-Bold.ttf",64);
	}

	myEditorContext = ImNodeEd::CreateEditor();

	if (myBgContextCategories.empty())
	{
		const auto supportedNodeTypes = GraphType::GetSupportedNodeTypes();
		for (const auto& [nodeName,nodeType] : supportedNodeTypes)
		{
			if (nodeType.NodeFlags & NODECLASS_FLAG_INTERNALONLY)
				continue;

			const std::string nodeCategory = nodeType.NodeCategory;
			if (auto catIt = myBgContextCategories.find(nodeCategory); catIt != myBgContextCategories.end())
			{
				catIt->second.Items.push_back({ nodeType.NodeTitle, nodeType.TypeName, "" });
			}
			else
			{
				myBgContextCategories.insert({ nodeCategory, {nodeCategory, {{ nodeType.NodeTitle, nodeType.TypeName }} } });
				myBgContextCategoryNamesList.push_back(nodeCategory);
			}
		}

		std::sort(myBgContextCategoryNamesList.begin(),myBgContextCategoryNamesList.end());
	}

	myEditorState.Search.bgCtxtSearchFieldResults.reserve(GraphType::GetSupportedNodeTypes().size());
	myEditorState.initNavToContent = true;
}

template<typename GraphType,typename GraphNodeType,typename GraphEdgeType,typename GraphPinType,typename SchemaType>
void GraphEditorBase<GraphType,GraphNodeType,GraphEdgeType,GraphPinType,SchemaType>::Render()
{
	ImNodeEd::SetCurrentEditor(myEditorContext);
	ImGui::SetNextWindowSize({ 1280, 720 },ImGuiCond_FirstUseEver);
	if (ImGui::Begin(myEditorState.editorTitle.c_str()))
	{
		RenderWindow();
		if (ImNodeEd::Begin("EditorContainer"))
		{
			RenderGraphEditor();
			ImNodeEd::End();
			ImVec2 tokenPos = ImGui::GetWindowContentRegionMax();
			ImGui::PushFont(ourEditorTokenFont);
			const ImVec2 tokenSize = ImGui::CalcTextSize(myEditorToken.c_str());
			tokenPos = { tokenPos.x - tokenSize.x - 24, tokenPos.y - tokenSize.y - 12 };

			ImGui::SetCursorPos(tokenPos);
			ImGui::PushStyleColor(ImGuiCol_Text,IM_COL32(255,255,255,128));
			ImGui::TextUnformatted(myEditorToken.c_str());
			ImGui::PopStyleColor();
			ImGui::PopFont();
		}
	}
	ImGui::End();
	ImNodeEd::SetCurrentEditor(nullptr);
}
