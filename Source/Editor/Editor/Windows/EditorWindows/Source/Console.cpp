#include "../Console.h"

#include "imgui.h"
#include "Tools/Logging/Logging.h"

void Console::RenderImGUi()
{
	ImGui::Begin("Console");
	{
		if (ImGui::BeginChild("ScrollingRegion"))
		{
			static int size = 0;
			for (const auto& [myColor,message] : Logger::m_LogMsgs)
			{
				if (message.empty())
				{
					break;
				}

				const auto color = ImVec4(myColor.x,myColor.y,myColor.z,1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text,color);
				ImGui::TextWrapped(message.c_str());
				ImGui::PopStyleColor();
			}
			if (size != Logger::m_LogMsgs.size())
			{
				ImGui::SetScrollHereY(1.0f);
				size = (int)Logger::m_LogMsgs.size();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
