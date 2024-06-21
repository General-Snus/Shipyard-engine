#include <Editor/Editor/Core/Editor.h>
#include <Tools/Reflection/refl.hpp>
#include "../Inspector.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "imgui.h" 
void Inspector::RenderImGUi()
{

	ImGui::Begin("Inspector");
	const auto& selectedGameObjects = Editor::GetSelectedGameObjects();

	if (!selectedGameObjects.empty())
	{
		auto gameobject = selectedGameObjects[0];






		for (auto& cmp : gameobject.GetAllComponents())
		{
			cmp->InspectorView();
			ImGui::Separator();
		}
	}
	ImGui::End();
}
