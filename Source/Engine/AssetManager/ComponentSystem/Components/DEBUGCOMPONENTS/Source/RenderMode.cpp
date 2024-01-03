#include "AssetManager.pch.h"
#include "../RenderMode.h"
#include <Tools/ImGui/ImGui/imgui.h> 
#include <Editor/Editor/Core/Editor.h> 

//TODO FIX THIS WITH EDITOR SETTINGS??
static inline std::string tonemaps[5] = {
	"No tonemap",
	"Tonemap_Reinhard2",
	"Tonemap_UnrealEngine",
	"Tonemap_ACES",
	"Tonemap_Lottes"
};


void RenderMode::Update()
{ 
	ImGui::Begin("Debug Filter"); 
	for(int i = 0; i < (int)DebugFilter::count; i++)
	{
		if(currentlyActiveLayer == i)
		{
			ImGui::RadioButton(ApplicationState::layerNames[i].c_str(),true);
		}
		else
		{
			if(ImGui::RadioButton(ApplicationState::layerNames[i].c_str(),false))
			{
				currentlyActiveLayer = i;
				Editor::GetApplicationState().filter = (DebugFilter)i;
			}
		}
	}
	ImGui::End();

	ImGui::Begin("Tonemap");
	for(int i = 0; i < 5; i++)
	{
		if(currentlyActiveTone == i)
		{
			ImGui::RadioButton(tonemaps[i].c_str(),true);
		}
		else
		{
			if(ImGui::RadioButton(tonemaps[i].c_str(),false))
			{
				currentlyActiveTone = i;

				GraphicsSettings& settings = GraphicsEngine::Get().GetSettings();
				settings.Tonemaptype = i;
			}
		}
	}
	ImGui::End();
}

void RenderMode::OnExit()
{
}

void RenderMode::OnStart()
{
}
