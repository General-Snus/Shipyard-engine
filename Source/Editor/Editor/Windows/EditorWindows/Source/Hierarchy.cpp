#include "../Hierarchy.h"

#include <format>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Font/IconsFontAwesome5.h>
#include "ImGuiHelpers.hpp"
#include "Editor/Editor/Commands/SceneAction.h"
#include "Editor/Editor/Core/Editor.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Engine/PersistentSystems/Scene.h"
#include "Tools/Utilities/Math.hpp"
#include "Tools/Utilities/Input/Input.hpp"

#undef min
#undef max

bool localActiveMenu = false;
bool clickedAnyNode = false;

void Hierarchy::PopupMenu(SY::UUID id) {
	OPTICK_EVENT();
	const auto popupId = "##Hierarchy" + static_cast<std::string>(id);
	if(ImGui::BeginPopupContextItem(popupId.c_str())) {
		localActiveMenu = true;

		ImGuiSelectableFlags flag{};

		if(!id.IsValid()) {
			flag |= ImGuiSelectableFlags_Disabled;
		}

		if(ImGui::Selectable("Create Empty")) {
			const auto ptr = std::make_shared<GameobjectAdded>(GameObject::Create("Empty"));
			CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		if(ImGui::Selectable("Create Empty Child",false,flag)) {
			const auto parent = Scene::activeManager().GetGameObject(id);
			auto       child = GameObject::Create("Empty child");
			child.transform().SetParent(parent.transform());

			const auto ptr = std::make_shared<GameobjectAdded>(child);
			CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		ImGui::Separator();
		if(ImGui::Selectable("Duplicate",false,flag)) {
			const auto gameObject = Scene::activeManager().GetGameObject(id);
			const auto components = gameObject.CopyAllComponents();

			auto newObject = GameObject::Create(gameObject.GetName() + "_1");
			for(const auto& i : components) {
				newObject.AddBaseComponent(i);
			}

			const auto ptr = std::make_shared<GameobjectAdded>(newObject);
			CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		if(ImGui::Selectable("Delete",false,flag)) {
			// TODO add do in add command function its more logical that way

			CommandPacket packet;
			GameObject    obj = Scene::activeManager().GetGameObject(id);
			const auto    parentCommand = std::make_shared<GameobjectDeleted>(obj);

			// TODO DELETE THE CILDS
			//  for (auto const &i : obj.transform().GetAllChildren())
			//{
			//      const auto ptr =
			//      std::make_shared<GameobjectDeleted>(i.get().gameObject());
			//      Scene::ActiveManager().DeleteGameObject(i.get().GetOwner(),
			//      true); packet.emplace_back(ptr);
			//  }
			packet.emplace_back(parentCommand);
			Scene::activeManager().DeleteGameObject(id,true);

			CommandBuffer::mainEditorCommandBuffer().addCommand(packet);
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		ImGui::Separator();
		if(ImGui::Selectable("Rename",false,flag)) {
			const auto gameObject = Scene::activeManager().GetGameObject(id);
			gameObject.SetName("Renamed GameObject");
		}
		ImGui::Separator();
		if(ImGui::Selectable("Copy")) {
			EDITOR_INSTANCE.Copy();
		}
		if(ImGui::Selectable("Paste")) {
			EDITOR_INSTANCE.Paste();
		}
		ImGui::Separator();
		if(ImGui::Selectable("Move camera to object",false,flag)) {
			EDITOR_INSTANCE.FocusObject(Scene::activeManager().GetGameObject(id));
		}
		if(ImGui::Selectable("Align With View",false,flag)) {
			// Move object to align with scene camera
			EDITOR_INSTANCE.AlignObject(Scene::activeManager().GetGameObject(id));
		}
		if(ImGui::Selectable("Align View to Selected",false,flag)) {
			// Move scene camera to align with selected object
			EDITOR_INSTANCE.FocusObject(Scene::activeManager().GetGameObject(id),false);
		}
		if(ImGui::Selectable("Set as Parent",false,flag)) {
			const auto selected = EDITOR_INSTANCE.GetSelectedGameObjects();

			for(auto& child : selected) {
				// TODO Whole component change function
				auto& parentTransform = Scene::activeManager().GetGameObject(id).transform();
				child.transform().SetParent(parentTransform);
			}
		}
		if(ImGui::Selectable("Clear Parent",false,flag)) {
			const auto gameObject = Scene::activeManager().GetGameObject(id);
			gameObject.transform().Detach();
		}
		ImGui::Separator();
		if(ImGui::Selectable("Add Component",false,flag)) {
		}
		ImGui::EndPopup();
	}
}

void Hierarchy::RenderNode(Transform& transform) {
	OPTICK_EVENT();
	auto flags = ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

	const auto& style = ImGui::GetStyle();
	auto        color = style.Colors[ImGuiCol_Text];

	const auto& data = transform.gameObject();
	const auto  id = data.GetID();
	bool        isSelected = false;
	const auto& selectedObjects = EDITOR_INSTANCE.GetSelectedGameObjects();
	for(const auto& i : selectedObjects) {
		if(i.GetID() == id) {
			isSelected = true;
			flags |= ImGuiTreeNodeFlags_Selected;
			break;
		}
	}

	ImGui::PushID(id);
	const bool hasChildren = transform.HasChildren();
	if(!hasChildren) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	{
		if(const bool isActive = data.GetActive(); !isActive) {
			color = style.Colors[ImGuiCol_TextDisabled];
		}

		ImGui::PushStyleColor(ImGuiCol_Text,color);

		const bool node_open =
			ImGui::TreeNodeEx((void*)static_cast<intptr_t>(id),flags,
				std::format("{} {}",ICON_FA_CUBE,data.GetName()).c_str());
		DragDrop(transform);
		if(ImGui::IsItemHovered()) {
			if(ImGui::IsItemClicked() && !isSelected || ImGui::IsItemJustReleased()) {
				clickedAnyNode = true;
				EDITOR_INSTANCE.m_Callbacks[EditorCallback::ObjectSelected].Invoke();

				auto& refSelected = EDITOR_INSTANCE.GetSelectedGameObjects();
				!Input.IsKeyHeld(Keys::SHIFT) ? refSelected.clear() : __nop();
				refSelected.push_back(Scene::activeManager().GetGameObject(id));
			}
		}
		PopupMenu(id);

		if(node_open) {
			if(hasChildren) {
				for(const auto& child : transform.GetAllDirectChildren()) {
					RenderNode(child);
				}
			}

			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}
	ImGui::PopID();
}

inline void Hierarchy::DragDrop(Transform& transform) {
	OPTICK_EVENT();
	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE")) {
			// TODO do we want all selected? We can guarantee that the payload is one of them
			const auto& list = EDITOR_INSTANCE.GetSelectedGameObjects();
			if(!list.empty()) {
				for(auto& obj : list) {
					if(!obj.IsValid() && obj.GetID() != transform.GetOwner()) {
						LOGGER.Warn("Invalid GameObject in hierachy_dragdrop");
						return;
					}

					obj.transform().SetParent(transform,true);
				}
			} else {
				const GameObject obj = *static_cast<GameObject*>(payload->Data);
				if(!obj.IsValid() && obj.GetID() != transform.GetOwner()) {
					LOGGER.Warn("Invalid GameObject in hierachy_dragdrop");
					return;
				}

				obj.transform().SetParent(transform,true);
			}
		}
		ImGui::EndDragDropTarget();
	}
	if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		auto       obj = transform.gameObject();
		const auto objCopy = GameObject(obj);
		ImGui::SetDragDropPayload("HIERARCHY_NODE",&objCopy,sizeof(GameObject));

		ImGui::Text(obj.GetName().c_str());

		if(ImGui::BeginTooltip()) {
			ImGui::Text(obj.GetName().c_str());
			ImGui::EndTooltip();
		}
		ImGui::EndDragDropSource();
	}
}

void Hierarchy::RenderImGUi() {
	OPTICK_EVENT();

	clickedAnyNode = false;
	localActiveMenu = false;
	const auto& gObjList = Scene::activeManager().GetAllGameObjects();
	ImGui::Begin("Hierarchy",&m_KeepWindow);

	static char buf[128] = "";
	{
		ImGui::BeginColumns("HierachySearchSpace",2);
		ImGui::SetColumnWidth(0,50);
		ImGui::SetColumnWidth(1,200);
		ImGui::Button("+");
		ImGui::NextColumn();

		ImGui::InputText("Search",buf,IM_ARRAYSIZE(buf));

		ImGui::EndColumns();
	}
	const std::string keyTerm = buf;

	ImGui::Separator();
	ImGui::BeginChild("GameObjectList");

	using localPair = std::pair<std::string,SY::UUID>;
	static std::vector<localPair> sortedList;

	for(const auto& [id,data] : gObjList) {
		OPTICK_EVENT("ObjectIteration");
		const auto& transform = Scene::activeManager().GetComponent<Transform>(id);
		if(transform.HasParent() || !data.IsVisibleInHierarcy) // We let parent handle the iterating over the children
		{
			continue;
		}

		sortedList.emplace_back(data.Name,id);
	}

	if(!keyTerm.empty()) {
		OPTICK_EVENT("Sorting");
		// sort the list
		std::ranges::sort(sortedList,[&keyTerm](const localPair& a,const localPair& b) {
			return Math::Levenshtein::distance(a.first,keyTerm) < Math::Levenshtein::distance(b.first,keyTerm);
			});
	}

	for(const auto& transformIndex : sortedList | std::ranges::views::values) {
		auto& transform = Scene::activeManager().GetComponent<Transform>(transformIndex);
		RenderNode(transform);
	}
	ImGui::EndChild();

	if(!localActiveMenu) {
		PopupMenu(SY::UUID::InvalidID());
	}

	if(!clickedAnyNode && ImGui::IsItemClicked()) {
		EDITOR_INSTANCE.GetSelectedGameObjects().clear();
	}

	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE")) {
			const auto& list = EDITOR_INSTANCE.GetSelectedGameObjects();
			if(!list.empty()) {
				for(auto& obj : list) {
					if(!obj.IsValid()) {
						LOGGER.Warn("Invalid GameObject in hierachy_dragdrop");
						return;
					}

					obj.transform().Detach();
				}
			} else {
				const GameObject obj = *static_cast<GameObject*>(payload->Data);
				if(!obj.IsValid()) {
					LOGGER.Warn("Invalid GameObject in hierachy_dragdrop");
					return;
				}

				obj.transform().Detach();
			}
		}
		ImGui::Text("Remove parent");
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
	sortedList.clear();
}
