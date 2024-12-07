#include "../History.h"
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Tools/Optick/include/optick.h>
#include <imgui.h>

void History::RenderImGUi()
{
    OPTICK_EVENT();
    ImGui::Begin("History", &m_KeepWindow);
    {
        // if (ImGui::BeginChild("ScrollingRegion"))
        {
            int size = 0;
            const int cursor = CommandBuffer::MainEditorCommandBuffer().GetCursor();
            for (const auto &command : CommandBuffer::MainEditorCommandBuffer().GetCommandList())
            {
                auto text = command.front()->GetDescription();
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
