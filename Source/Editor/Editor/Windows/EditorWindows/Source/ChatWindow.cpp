#include "../ChatWindow.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <Engine/PersistentSystems/Networking/NetMessage/ChatMessage.h>
#include <Engine/PersistentSystems/Networking/Client/Client.h>
#include <Tools/Reflection/refl.hpp>

ChatWindow::ChatWindow()
{
	m_Client = new Client();
	m_Client->Setup();
	ZeroMemory(currentMessage, 512);
}

ChatWindow::~ChatWindow() { delete m_Client; }

void ChatWindow::RenderImGUi()
{
	ImGui::Begin(std::format("Chat window##{}",uniqueID).c_str(),&m_KeepWindow);
	m_Client->Update();

	if (m_Client->HasRecievedMessage() && m_Client->GetType() == eNetMessageType::ChatMessage)
	{
		m_ChatMessages.emplace_back(m_Client->GetLatestMessageAsType<ChatMessage>()->ReadMessage());
	}

	for (const auto &message : m_ChatMessages)
	{
		ImGui::Text(message.c_str());
	}

	ImGui::InputText("Message", currentMessage, MAX_NETMESSAGE_SIZE);
	ImGui::SameLine();

	if (const std::string message = currentMessage;
		ImGui::Button("Send") && !message.empty())
	{
		m_Client->Send(message);
		ZeroMemory(currentMessage, 512);
	}
	ImGui::End();
}
