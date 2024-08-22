#include "../Console.h"

#include <Tools/Optick/include/optick.h>

#include "imgui.h"

void Console::RenderImGUi()
{
    OPTICK_EVENT();
    ImGui::Begin("Console", &m_KeepWindow);
    { // TODO clipper
        using enum Logger::LogType;
        const auto &style = ImGui::GetStyle();

        auto getButtonColor = [&](Logger::LogType val, Logger::LogType buttonType) {
            if ((val & buttonType) == none)
            {
                return style.Colors[ImGuiCol_::ImGuiCol_Button];
            }
            else
            {
                return style.Colors[ImGuiCol_::ImGuiCol_ButtonActive];
            }
        };

        if (ImGui::ColorButton("All", getButtonColor(filter, All)))
        {

            if (filter == All)
            {
                filter = none;
            }
            if (filter != All)
            {
                filter = All;
            }
        }
        ImGui::SameLine();
        ImGui::Text(std::format("All: {}", Logger::m_Buffer.LoggedMessages.size()).c_str());
        ImGui::SameLine();

        ImGui::ColorButton("Messages:", getButtonColor(filter, message)) ? filter ^= message : none;
        ImGui::SameLine();
        ImGui::Text(std::format("Messages: {}", Logger::m_Buffer.messagesCount).c_str());
        ImGui::SameLine();

        ImGui::ColorButton("Warnings: ", getButtonColor(filter, warning)) ? filter ^= warning : none;
        ImGui::SameLine();
        ImGui::Text(std::format("Warnings: {}", Logger::m_Buffer.warnCount).c_str());
        ImGui::SameLine();

        ImGui::ColorButton("Error: ", getButtonColor(filter, error)) ? filter ^= error : none;
        ImGui::SameLine();
        ImGui::Text(std::format("Error: {}", Logger::m_Buffer.errCount).c_str());
        ImGui::SameLine();

        ImGui::ColorButton("Critical: ", getButtonColor(filter, critical)) ? filter ^= critical : none;
        ImGui::SameLine();
        ImGui::Text(std::format("Critical: {}", Logger::m_Buffer.criticalCount).c_str());
        ImGui::SameLine();

        ImGui::ColorButton("Success: ", getButtonColor(filter, success)) ? filter ^= success : none;
        ImGui::SameLine();
        ImGui::Text(std::format("Success: {}", Logger::m_Buffer.successCount).c_str());
        ImGui::SameLine();

        if (ImGui::Button("Clear"))
        {
            Logger::m_Buffer.LoggedMessages.clear();
            Logger::m_Buffer.criticalCount = 0;
            Logger::m_Buffer.errCount = 0;
            Logger::m_Buffer.messagesCount = 0;
            Logger::m_Buffer.successCount = 0;
            Logger::m_Buffer.warnCount = 0;
        }

        if (ImGui::BeginChild("ScrollingRegion"))
        {
            static int size = 0;
            for (const auto &[type, message] : Logger::m_Buffer.LoggedMessages)
            {
                if (message.empty() || (filter & type) == none)
                {
                    continue;
                }

                Color logColor = Logger::GetColor(type);
                const auto color = logColor.GetRGBA();
                const ImVec4 &textColor = color;
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                ImGui::TextWrapped(message.c_str());
                ImGui::PopStyleColor();
            }
            if (size != Logger::m_Buffer.LoggedMessages.size())
            {
                ImGui::SetScrollHereY(1.0f);
                size = (int)Logger::m_Buffer.LoggedMessages.size();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
