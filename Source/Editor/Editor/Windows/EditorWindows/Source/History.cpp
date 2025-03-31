#include "../History.h"
#include <imgui.h>
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <External/Optick/include/optick.h>

void History::RenderImGUi()
{
	OPTICK_EVENT();
	ImGui::Begin("History", &m_KeepWindow);
	{
		// if (ImGui::BeginChild("ScrollingRegion"))
		{
			size_t     size = 0;
			const auto cursor = CommandBuffer::mainEditorCommandBuffer().getCursor();
			for (const auto& command : CommandBuffer::mainEditorCommandBuffer().getCommandList())
			{
				auto text = command.front()->getDescription();
				if (!command.front())
				{
					continue;
				}
				if (size == cursor)
				{
					// Highlight current command
					// ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f,1.0f,0.0f,1.0f);
					// ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.0f,1.0f,0.0f,1.0f));
					// ImGui::PopStyleColor();
					ImGui::TextWrapped(text.c_str());
				}
				else
				{
					ImGui::TextDisabled(text.c_str());
				}
				size++;
			}
		}
		// ImGui::EndChild();
	}
	ImGui::End();
}
