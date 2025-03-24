#include "../Console.h"

#include <Tools/Optick/include/optick.h>

#include <ranges>
#include "imgui.h"

void Console::RenderImGUi() {
	OPTICK_EVENT();
	ImGui::Begin("Console",&m_KeepWindow,ImGuiWindowFlags_NoResize);
	{
		// TODO clipper
		using enum LoggerService::LogType;
		const auto& style = ImGui::GetStyle();
		const auto& buffer = LOGGER.m_Buffer;
		auto        getButtonColor = [&](LoggerService::LogType val,LoggerService::LogType buttonType) {
			if((val & buttonType) == none) {
				return style.Colors[ImGuiCol_Button];
			}
			return style.Colors[ImGuiCol_ButtonActive];
		};

		if(ImGui::ColorButton("All",getButtonColor(filter,All))) {
			if(filter == All) {
				filter = none;
			}
			if(filter != All) {
				filter = All;
			}
		}
		ImGui::SameLine();
		ImGui::Text(std::format("All: {}",buffer.messagesCount +buffer.warnCount + buffer.errCount + buffer.criticalCount + buffer.successCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Messages:",getButtonColor(filter,message)) ? filter ^= message : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Messages: {}",buffer.messagesCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Warnings: ",getButtonColor(filter,warning)) ? filter ^= warning : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Warnings: {}",buffer.warnCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Error: ",getButtonColor(filter,error)) ? filter ^= error : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Error: {}",buffer.errCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Critical: ",getButtonColor(filter,critical)) ? filter ^= critical : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Critical: {}",buffer.criticalCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Success: ",getButtonColor(filter,success)) ? filter ^= success : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Success: {}",buffer.successCount).c_str());
		ImGui::SameLine();

		if(ImGui::Button("Clear")) {
			std::scoped_lock lock(LOGGER.mutexLock());
			LOGGER.Clear();
			pickedMessage = LoggerService::LogMsg();
		}

		const Vector2f spaceAvail = {ImGui::GetContentRegionAvail().x,ImGui::GetContentRegionAvail().y * 0.75f};
		if(ImGui::BeginChild("ScrollingRegion",static_cast<ImVec2>(spaceAvail))) {
			int index = 0;
			std::scoped_lock lock(LOGGER.mutexLock());
			for(const auto& logEntity : buffer.LoggedMessages) {
				const auto& [type,message,trace] = logEntity;
				index ++;

				if(message.empty() || (filter & type) == none) {
					continue;
				}

				Color         logColor = LOGGER.GetColor(type);
				const auto    color = logColor.GetRGBA();
				const ImVec4& textColor = color;
				ImGui::PushStyleColor(ImGuiCol_Text,textColor);
				Vector2f space = {ImGui::GetContentRegionAvail().x,ImGui::GetTextLineHeight()};

				ImGui::Separator();
				auto textBeginCursor = ImGui::GetCursorPos();
				ImGui::Text(message.c_str());

				auto textArea = ImGui::GetItemRectSize();
				auto textEndCursor = ImGui::GetCursorPos();

				ImGui::SetCursorPos(textBeginCursor);
				if(ImGui::InvisibleButton(std::format("ConsoleChildWindowButton{}",index).c_str(),textArea)) {
					pickedMessage = logEntity;
				}

				ImGui::SetCursorPos(textEndCursor);

				ImGui::PopStyleColor();
			}

			static int size = 0;
			if(size != buffer.LoggedMessages.size()) {
				ImGui::SetScrollHereY(1.0f);
				size = static_cast<int>(buffer.LoggedMessages.size());
			}
		}
		ImGui::EndChild();

		if(ImGui::BeginChild("DetailView")) {
			ImGui::TextWrapped(pickedMessage.message.c_str());
			ImGui::Spacing();
			for(auto& traceElement : pickedMessage.trace) {
				ImGui::TextWrapped(traceElement.description().c_str());
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
