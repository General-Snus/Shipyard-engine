#include <Editor/Editor/Core/Editor.h> 
#include <Tools/Reflection/refl.hpp>
#include "../Inspector.h"


#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/Enums.h"
#include "imgui.h" 
#include "Tools/Logging/Logging.h"
#include "Tools/Utilities/TemplateHelpers.h"

Inspector::Inspector()
{
	Editor::Get().m_Callbacks[EditorCallback::ObjectSelected].AddListener(std::bind(&Inspector::ToFront,this));
}

void Inspector::ToFront()
{
	Logger::Log("Inspector::ToFront");
}

void Inspector::RenderImGUi()
{
	OPTICK_EVENT();

	ImGui::Begin("Inspector");
	const auto& selectedGameObjects = Editor::GetSelectedGameObjects();

	if (!selectedGameObjects.empty())
	{
		auto gameobject = selectedGameObjects[0];

		ImGui::PushItemWidth(60);
		if (bool activeStatus = gameobject.GetActive(); ImGui::Checkbox("##",&activeStatus))
		{
			gameobject.SetActive(activeStatus);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushItemWidth(200);

		char strName[128] = {};
		strcpy_s(strName,gameobject.GetName().c_str());
		if (ImGui::InputText("##Name",strName,IM_ARRAYSIZE(strName)))
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
		constexpr int enumCount = (int)magic_enum::enum_count<Layer>();


		std::vector<const char*> testArray;
		for_sequence<enumCount>([&testArray](auto i) constexpr
			{
				testArray.emplace_back(layers[i].data());
			});

		const int flagLayer = (int)gameobject.GetLayer();
		int currentLayer = (int)std::log2(flagLayer);
		if (ImGui::Combo("##Tag",&currentLayer,testArray.data(),enumCount))
		{
			gameobject.SetLayer((Layer)(1 << currentLayer));
		}
		ImGui::PopItemWidth();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		for (auto& cmp : gameobject.GetAllComponents())
		{
			cmp->InspectorView();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
		}
	}
	ImGui::End();
}
