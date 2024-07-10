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
			transform;
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
		ImGui::Begin("Hierarchy",&m_KeepWindow);
		ImGui::Separator();
		ImGui::BeginChild("GameObjectList");
		for (const auto& [id,data] : gObjList)
		{
			const auto& transform = Scene::ActiveManager().GetComponent<Transform>(id); 

			if (!data.IsVisibleInHierarcy || transform.HasParent()) //We let parent handle the iterating over the children
			{
				continue;
			}

			RenderNode(transform);
		}
		ImGui::EndChild();


		ImGui::End();
	}
}
