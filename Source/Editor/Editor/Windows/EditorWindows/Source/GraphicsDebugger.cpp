#include "../GraphicsDebugger.h"

#include "imgui.h"

#include <Core/Editor.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Windows/EditorWindows/Viewport.h>

void GraphicsDebugger::RenderImGUi()
{
	ImGui::Begin("Debug Filter", &m_KeepWindow);

	constexpr auto& debugFilter = magic_enum::enum_names<DebugFilter>();
	constexpr auto  enumCount = magic_enum::enum_count<DebugFilter>();

	std::vector<const char*> testArray;
	for_sequence<enumCount>([&testArray](auto i) constexpr { testArray.emplace_back(debugFilter[i].data()); });

	for (int i = 0; i < static_cast<int>(DebugFilter::count); i++)
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
				if (const auto viewport = EDITOR_INSTANCE.GetMainViewport())
				{
					viewport->GetCamera().filter = static_cast<DebugFilter>(currentlyActiveLayer);
				}
			}
		}
	}
	ImGui::End();
}
