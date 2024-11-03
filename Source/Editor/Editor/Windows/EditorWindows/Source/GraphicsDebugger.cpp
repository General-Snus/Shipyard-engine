#include "../GraphicsDebugger.h"

#include "imgui.h"

#include <Core/Editor.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Windows/EditorWindows/Viewport.h>

void GraphicsDebugger::RenderImGUi()
{
	ImGui::Begin("Debug Filter", &m_KeepWindow);

	constexpr auto &debugFilter = magic_enum::enum_names<DebugFilter>();
	constexpr int enumCount = (int)magic_enum::enum_count<DebugFilter>();

	std::vector<const char *> testArray;
	for_sequence<enumCount>([&testArray](auto i) constexpr { testArray.emplace_back(debugFilter[i].data()); });

	for (int i = 0; i < (int)DebugFilter::count; i++)
	{
		if (currentlyActiveLayer == i)
		{
			ImGui::RadioButton(testArray[i], true);
		}
		else
		{
			if (ImGui::RadioButton(testArray[i], false))
			{
				currentlyActiveLayer = i;
				if (auto viewport = EditorInstance.GetMainViewport())
				{
					viewport->GetCamera().GetComponent<Camera>().filter = (DebugFilter)currentlyActiveLayer;
				}
			}
		}
	}
	ImGui::End();
}
