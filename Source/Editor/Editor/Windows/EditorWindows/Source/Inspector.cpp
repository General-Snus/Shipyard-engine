#include "../Inspector.h"
#include <Editor/Editor/Core/Editor.h>
#include <Tools/Reflection/refl.hpp>

#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/Enums.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/TemplateHelpers.h" 
#include "imgui.h"
#include "imgui_internal.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/PersistentSystems/SceneUtilities.h>

void Inspector::Initialize()
{
	GetEditor().m_Callbacks[EditorCallback::ObjectSelected].AddListener([this]() { this->ToFront(); });
}

void Inspector::ToFront()
{
	toFront = true;
}

void Inspector::RenderImGUi()
{
	OPTICK_EVENT();

	if (toFront)
	{
		ImGui::SetNavWindow(ImGui::GetCurrentWindow());
		toFront = false;
	}

	GameObject gameobject;
	const auto& selectedGameObjects = GetEditor().GetSelectedGameObjects();
	if (!selectedGameObjects.empty())
	{
		gameobject = selectedGameObjects[0];

		ImGui::PushItemWidth(60);
		if (bool activeStatus = gameobject.GetActive(); ImGui::Checkbox("##", &activeStatus))
		{
			gameobject.SetActive(activeStatus);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushItemWidth(200);

		char strName[128] = {};
		strcpy_s(strName, gameobject.GetName().c_str());
		if (ImGui::InputText("##Name", strName, IM_ARRAYSIZE(strName)))
		{
			gameobject.SetName(strName);
		}
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(60);
		ImGui::Text("Tag: ");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushItemWidth(200);

		constexpr auto& layers = magic_enum::enum_names<Layer>();
		constexpr auto  enumCount = magic_enum::enum_count<Layer>();

		std::vector<const char*> testArray;
		for_sequence<enumCount>([&testArray](auto i) constexpr { testArray.emplace_back(layers[i].data()); });

		const auto flagLayer = static_cast<int>(gameobject.GetLayer());
		int       currentLayer = static_cast<int>(std::log2(flagLayer));
		if (ImGui::Combo("##Tag", &currentLayer, testArray.data(), enumCount))
		{
			gameobject.SetLayer(static_cast<Layer>(1 << currentLayer));
		}
		ImGui::PopItemWidth();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		auto& transform = gameobject.transform();
		ImGui::PushID(&transform);
		transform.InspectorView();
		ImGui::PopID();

		ImGui::NewLine();
		for (const auto& cmp : gameobject.GetAllComponents())
		{
			if (cmp->GetTypeInfo().Name() == refl::type_descriptor<Transform>::name.str())
			{
				continue;
			}
			ImGui::PushID(cmp);
			cmp->InspectorView();
			ImGui::PopID();
			ImGui::NewLine();
		}

		ImGui::NewLine();
	}
}
