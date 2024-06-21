#include "../Hierarchy.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Editor/Editor/Core/Editor.h" 
#include  <format>
#include "ImGuiHepers.hpp"
#include "Tools/Utilities/Input/Input.hpp"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

#include "Engine/PersistentSystems/Scene.h"

void Hierarchy::RenderImGUi()
{
	{
		const auto& gObjList = Scene::ActiveManager().GetAllGameObjects();
		ImGui::Begin("Hierarchy");
		ImGui::Separator();
		ImGui::BeginChild("GameObjectList");
		for (const auto& [id,data] : gObjList)
		{
			if (!data.IsVisibleInHierarcy)
			{
				continue;
			}
			ImGui::PushID(id);
			auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
			const auto& transform = Scene::ActiveManager().GetComponent<Transform>(id);

			if (!transform.HasChildren())
			{
				flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
			}

			{
				bool arg = data.IsActive;
				if (ImGui::TreeNodeEx(std::format("##{}",data.Name).c_str(),flags))
				{
					ImGui::SameLine(); 
					auto gameObjectWidget = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/GameObject.png");
					const auto height = ImGui::GetFrameHeight();
					ImGui::Image(gameObjectWidget,{ height,height });
					ImGui::SameLine();

					auto color = ImVec4(1.0f,1.0f,1.0f,1.0f);
					if (ImGui::IsItemHovered())
					{
						color = ImVec4(0.0f,1.0f,0.0f,1.0f);

						if (ImGui::IsItemClicked())
						{
							auto& refSelected = Editor::GetSelectedGameObjects();
							!Input::IsKeyHeld(Keys::SHIFT) ? refSelected.clear() : __nop();
							refSelected.push_back(Scene::ActiveManager().GetGameObject(id));
						}

					}

					if (!arg)
					{
						color.x *= .5f;
						color.y *= .5f;
						color.z *= .5f;
					}

					ImGui::PushStyleColor(ImGuiCol_Text,color);
					ImGui::TextWrapped(data.Name.c_str());
					ImGui::PopStyleColor();
					ImGui::TreePop();

				}
			}
			ImGui::PopID();
		}
		ImGui::EndChild();


		ImGui::End();
	}
}
