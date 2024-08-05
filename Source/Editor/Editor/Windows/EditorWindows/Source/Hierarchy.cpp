#include "../Hierarchy.h"

#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include  <format>
#include "Editor/Editor/Core/Editor.h" 
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "ImGuiHepers.hpp"
#include "Tools/Utilities/Input/Input.hpp"

#include "Engine/PersistentSystems/Scene.h"

#undef min
#undef max

void Hierarchy::PopupMenu(SY::UUID id)
{
	const auto popupId = "##Hierarchy" + static_cast<std::string>(id);
	if (ImGui::BeginPopupContextItem(popupId.c_str()))
	{
		if (ImGui::Selectable("Create Empty"))
		{
			GameObject::Create("Empty");
		}
		if (ImGui::Selectable("Create Empty Child"))
		{
			auto parent = Scene::ActiveManager().GetGameObject(id);
			auto child = GameObject::Create("Empty");
			child.transform().SetParent(parent.transform());
		}
		ImGui::Separator();
		if (ImGui::Selectable("Duplicate"))
		{
			auto gameObject = Scene::ActiveManager().GetGameObject(id);
			auto components = gameObject.CopyAllComponents();

			auto newObject = GameObject::Create(gameObject.GetName() + "_1");
			for (auto& i : components)
			{
				newObject.AddBaseComponent(i);
			}
		}
		if (ImGui::Selectable("Delete"))
		{
			Scene::ActiveManager().DeleteGameObject(id);
		}
		ImGui::Separator();
		if (ImGui::Selectable("Rename"))
		{
			auto gameObject = Scene::ActiveManager().GetGameObject(id);
			gameObject.SetName("Renamed GameObject");
		}
		ImGui::Separator();
		if (ImGui::Selectable("Copy"))
		{
		}
		if (ImGui::Selectable("Paste"))
		{
		}
		ImGui::Separator();
		if (ImGui::Selectable("Move camera to object"))
		{
			Editor::Get().FocusObject(Scene::ActiveManager().GetGameObject(id));
		}
		if (ImGui::Selectable("Align With View"))
		{
			//Move object to align with scene camera
			Editor::Get().AlignObject(Scene::ActiveManager().GetGameObject(id));
		}
		if (ImGui::Selectable("Align View to Selected"))
		{
			//Move scene camera to align with selected object
			Editor::Get().FocusObject(Scene::ActiveManager().GetGameObject(id), false);
		}
		if (ImGui::Selectable("Set as Parent"))
		{
			auto selected = Editor::GetSelectedGameObjects();
			for (auto& child : selected)
			{
				child.GetComponent<Transform>().SetParent(Scene::ActiveManager().GetGameObject(id).GetComponent<Transform>());
			}
		}
		if (ImGui::Selectable("Clear Parent"))
		{
			auto gameObject = Scene::ActiveManager().GetGameObject(id);
			gameObject.transform().Detach();
		}
		ImGui::Separator();
		if (ImGui::Selectable("Add Component"))
		{

		}
		ImGui::EndPopup();
	}
}

void Hierarchy::RenderNode(const Transform& transform)
{
	const auto& data = transform.GetGameObject();

	auto id = data.GetID();
	ImGui::PushID(id);
	auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
	bool hasChildren = transform.HasChildren();
	if (!hasChildren)
	{
		flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
	}

	{
		bool isActive = data.GetActive();
		if (ImGui::TreeNodeEx(std::format("##{}", data.GetName()).c_str(), flags))
		{
			ImGui::SameLine();
			auto gameObjectWidget = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/GameObject.png");
			const auto height = ImGui::GetFrameHeight();
			ImGui::Image(gameObjectWidget, { height,height });
			ImGui::SameLine();
			const auto& style = ImGui::GetStyle();

			auto color = style.Colors[ImGuiCol_Text];
			if (ImGui::IsItemHovered())
			{
				color = style.Colors[ImGuiCol_CheckMark];

				if (ImGui::IsItemClicked())
				{
					Editor::Get().m_Callbacks[EditorCallback::ObjectSelected].Invoke();


					auto& refSelected = Editor::GetSelectedGameObjects();
					!Input::IsKeyHeld(Keys::SHIFT) ? refSelected.clear() : __nop();
					refSelected.push_back(Scene::ActiveManager().GetGameObject(id));
				}
			}
			PopupMenu(id);

			if (!isActive)
			{
				color = style.Colors[ImGuiCol_TextDisabled];
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextWrapped(data.GetName().c_str());
			ImGui::PopStyleColor();

			if (hasChildren)
			{
				for (auto& child : transform.GetAllDirectChildren())
				{
					RenderNode(child);
				}
			}

			ImGui::TreePop();

		}
	}
	ImGui::PopID();
}

void Hierarchy::RenderImGUi()
{
	OPTICK_EVENT();
	{
		const auto& gObjList = Scene::ActiveManager().GetAllGameObjects();
		ImGui::Begin("Hierarchy", &m_KeepWindow);

		static char buf[128] = "";
		{
			ImGui::BeginColumns("HierachySearchSpace", 2);
			ImGui::SetColumnWidth(0, 50);
			ImGui::SetColumnWidth(1, 200);
			ImGui::Button("+");
			ImGui::NextColumn();

			ImGui::InputText("Search", buf, IM_ARRAYSIZE(buf));

			ImGui::EndColumns();
		}
		const std::string keyTerm = buf;


		ImGui::Separator();
		ImGui::BeginChild("GameObjectList");


		auto Levenshtein = [](const std::string& s1, const std::string& s2)
			{
				const size_t len1 = s1.size(), len2 = s2.size();
				std::vector<std::vector<size_t>> d(len1 + 1, std::vector<size_t>(len2 + 1));

				d[0][0] = 0;
				for (size_t i = 1; i <= len1; ++i) d[i][0] = i;
				for (size_t i = 1; i <= len2; ++i) d[0][i] = i;

				for (size_t i = 1; i <= len1; ++i)
				{
					for (size_t j = 1; j <= len2; ++j)
					{
						d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
					}
				}

				return d[len1][len2];
			};


		using localPair = std::pair<std::string, SY::UUID>;
		static std::vector<localPair> sortedList;

		for (const auto& [id, data] : gObjList)
		{
			const auto& transform = Scene::ActiveManager().GetComponent<Transform>(id);
			if (!data.IsVisibleInHierarcy || transform.HasParent()) //We let parent handle the iterating over the children
			{
				continue;
			}


			if (!keyTerm.empty() && Levenshtein(data.Name, keyTerm) >= std::max(data.Name.length(), keyTerm.length()))
			{
				continue;
			}

			sortedList.emplace_back(data.Name, id);
		}

		if (!keyTerm.empty())
		{
			//sort the list
			std::ranges::sort(sortedList, [=](const localPair& a, const localPair& b)
				{
					return Levenshtein(a.first, keyTerm) < Levenshtein(b.first, keyTerm);
				});
		}

		for (const auto& transformIndex : sortedList | std::ranges::views::values)
		{
			const auto& transform = Scene::ActiveManager().GetComponent<Transform>(transformIndex);
			RenderNode(transform);
		}

		ImGui::EndChild();


		ImGui::End();
		sortedList.clear();
	}
}
