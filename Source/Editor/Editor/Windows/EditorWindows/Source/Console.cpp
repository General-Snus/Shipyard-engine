#include "../Console.h"

#include <External/Optick/include/optick.h>
#include <DirectX\DX12\Graphics\GPU.h> 

#include <ranges>
#include "imgui.h"

void Console::RenderImGUi()
{
	OPTICK_EVENT();


	PushDebugLayerToConsole();

	ImGui::Begin("Console", &m_KeepWindow, ImGuiWindowFlags_NoResize);
	{
		// TODO clipper
		using enum LoggerService::LogType;
		const auto& style = ImGui::GetStyle();
		const auto& buffer = LOGGER.m_Buffer;
		auto        getButtonColor = [&](LoggerService::LogType val, LoggerService::LogType buttonType)
		{
			if ((val & buttonType) == none)
			{
				return style.Colors[ImGuiCol_Button];
			}
			return style.Colors[ImGuiCol_ButtonActive];
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
		ImGui::Text(std::format("All: {}", buffer.messagesCount + buffer.warnCount + buffer.errCount + buffer.criticalCount + buffer.successCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Messages:", getButtonColor(filter, message)) ? filter ^= message : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Messages: {}", buffer.messagesCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Warnings: ", getButtonColor(filter, warning)) ? filter ^= warning : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Warnings: {}", buffer.warnCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Error: ", getButtonColor(filter, error)) ? filter ^= error : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Error: {}", buffer.errCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Critical: ", getButtonColor(filter, critical)) ? filter ^= critical : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Critical: {}", buffer.criticalCount).c_str());
		ImGui::SameLine();

		ImGui::ColorButton("Success: ", getButtonColor(filter, success)) ? filter ^= success : none;
		ImGui::SameLine();
		ImGui::Text(std::format("Success: {}", buffer.successCount).c_str());
		ImGui::SameLine();

		if (ImGui::Button("Clear"))
		{
			std::scoped_lock lock(LOGGER.mutexLock());
			LOGGER.Clear();
			pickedMessage = LoggerService::LogMsg();
		}

		const Vector2f spaceAvail = { ImGui::GetContentRegionAvail().x,ImGui::GetContentRegionAvail().y * 0.75f };
		if (ImGui::BeginChild("ScrollingRegion", static_cast<ImVec2>(spaceAvail)))
		{
			std::scoped_lock lock(LOGGER.mutexLock());
			const auto& messages = buffer.LoggedMessages;

			ImGuiListClipper clipper;
			clipper.Begin((int)messages.size(), 2 * ImGui::GetTextLineHeightWithSpacing());

			while (clipper.Step())
			{

				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					const auto& logEntity = messages[i];
					const auto& [type, message, trace, nr] = logEntity;

					if (message.empty() || (filter & type) == none)
					{
						continue;
					}

					// Push color and text
					Color logColor = LOGGER.GetColor(type);
					ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(logColor.GetRGBA()));

					ImGui::PushID(i);
					if (ImGui::Selectable(message.c_str(), pickedMessage.logNumber == logEntity.logNumber))
					{
						pickedMessage = logEntity;
					}
					ImGui::PopID();

					ImGui::PopStyleColor();
				}
			}
			clipper.End();

			// Auto-scroll logic
			static int lastSize = 0;
			if (lastSize != messages.size() && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1.0f);
			}
			lastSize = static_cast<int>(messages.size());
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("DetailView"))
		{
			ImGui::TextWrapped(pickedMessage.message.c_str());
			ImGui::Spacing();
			for (auto& traceElement : pickedMessage.trace)
			{
				ImGui::TextWrapped(traceElement.description().c_str());
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void Console::PushDebugLayerToConsole()
{
	SIZE_T messageLength = 0;
	auto pInfoQueue = GetGPU().QueryInfoQueue();
	if (pInfoQueue)
	{
		static std::vector<byte> bytes;
		for (size_t i = 0; i < pInfoQueue->GetNumStoredMessages(); i++)
		{
			HRESULT hr = pInfoQueue->GetMessage(i, NULL, &messageLength);
			if (FAILED(hr)) { continue; }
			// Allocate space and get the message
			bytes.resize(messageLength);
			D3D12_MESSAGE* pMessage = (D3D12_MESSAGE*)bytes.data();
			hr = pInfoQueue->GetMessage(i, pMessage, &messageLength);
			if (FAILED(hr)) { continue; }

			std::string str;
			str.reserve(pMessage->DescriptionByteLength);
			str.assign(pMessage->pDescription, pMessage->DescriptionByteLength);

			switch (pMessage->Severity)
			{
			case D3D12_MESSAGE_SEVERITY_CORRUPTION:
				LOGGER.Err(str);
				break;
			case D3D12_MESSAGE_SEVERITY_ERROR:
				LOGGER.Err(str);
				break;
			case D3D12_MESSAGE_SEVERITY_WARNING:
				LOGGER.Warn(str);
				break;
				//case D3D12_MESSAGE_SEVERITY_INFO:
				//	LOGGER.Log(str);
				//	break;
				//case D3D12_MESSAGE_SEVERITY_MESSAGE:
				//	LOGGER.Log(str);
				break;
			default:
				break;
			}
		}
		pInfoQueue->ClearStoredMessages();
	};
}
