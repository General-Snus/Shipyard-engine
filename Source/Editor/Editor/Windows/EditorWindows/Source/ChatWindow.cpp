#include "../ChatWindow.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <Networking/NetShared/NetMessage/ChatMessage.h>

ChatWindow::ChatWindow()
{
	m_Client.Setup();
	ZeroMemory(currentMessage, 512);
}

void ChatWindow::RenderImGUi()
{

	std::string id = std::to_string(ImGui::GetCurrentWindow()->IDStack.back());

	ImGui::Begin(std::format("Chat Window##{}", id).c_str(), &m_KeepWindow);
	m_Client.Update();

	if (m_Client.HasRecievedMessage() && m_Client.GetType() == eNetMessageType::ChatMessage)
	{
		m_ChatMessages.emplace_back(m_Client.GetLatestMessageAsType<ChatMessage>()->ReadMessage());
	}

	for (const auto &message : m_ChatMessages)
	{
		ImGui::Text(message.c_str());
	}

	ImGui::InputText(std::format("Message##{}", id).c_str(), currentMessage, MAX_NETMESSAGE_SIZE);
	ImGui::SameLine();

	if (const std::string message = currentMessage;
		ImGui::Button(std::format("Send##{}", id).c_str()) && !message.empty())
	{
		m_Client.Send(message);
		ZeroMemory(currentMessage, 512);
	}
	ImGui::End();
}
