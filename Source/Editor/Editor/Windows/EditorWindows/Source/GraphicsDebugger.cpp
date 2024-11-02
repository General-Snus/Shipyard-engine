#include "../GraphicsDebugger.h"

#include "imgui.h"
// #include <Engine/GraphicsEngine/GraphicsEngine.h>

static inline std::array<std::string, 5> tonemaps = {"No tonemap", "Tonemap_Reinhard2", "Tonemap_UnrealEngine",
													 "Tonemap_ACES", "Tonemap_Lottes"};

void GraphicsDebugger::RenderImGUi()
{
	ImGui::Begin("Debug Filter", &m_KeepWindow);
	for (int i = 0; i < (int)DebugFilter::count; i++)
	{
		if (currentlyActiveLayer == i)
		{
			ImGui::RadioButton(ApplicationState::layerNames[i].c_str(), true);
		}
		else
		{
			if (ImGui::RadioButton(ApplicationState::layerNames[i].c_str(), false))
			{
				currentlyActiveLayer = i;
				ApplicationState::filter = static_cast<DebugFilter>(i);
			}
		}
	}
	ImGui::Separator();
	for (int i = 0; i < 5; i++)
	{
		if (currentlyActiveTone == i)
		{
			ImGui::RadioButton(tonemaps[i].c_str(), true);
		}
		else
		{
			if (ImGui::RadioButton(tonemaps[i].c_str(), false))
			{
				currentlyActiveTone = i;

				// GraphicsSettings &settings = GraphicsEngineInstance.GetSettings();
				// settings.Tonemaptype = i;
			}
		}
	}
	ImGui::End();
}
