#include "AssetManager.pch.h"
#include "RenderMode.h"
#include <Tools/ImGui/imgui.h> 
#include <Game/Modelviewer/Core/Modelviewer.h>

void RenderMode::Update()
{ 
	ImGui::Begin("Debug Filter"); 
	for(int i = 0; i < (int)DebugFilter::count; i++)
	{
		if(currentlyActive == i)
		{
			ImGui::RadioButton(ApplicationState::layerNames[i].c_str(),true);
		}
		else
		{
			if(ImGui::RadioButton(ApplicationState::layerNames[i].c_str(),false))
			{
				currentlyActive = i;
				ModelViewer::GetApplicationState().filter = (DebugFilter)i;
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
