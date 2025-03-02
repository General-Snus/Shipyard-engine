#include "../ChatWindow.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <Engine/PersistentSystems/Networking/NetMessage/ChatMessage.h>
#include <Engine/PersistentSystems/Networking/Client.h>
#include <Tools/Reflection/refl.hpp>
#include <Engine/PersistentSystems/Networking/Server.h> 

ChatWindow::ChatWindow()
{
	//m_Client = new Client();
	//m_Client->Setup();
	
	currentMessage.resize(512);
}

ChatWindow::~ChatWindow() = default;

void ChatWindow::RenderImGUi()
{
	ImGui::Begin(std::format("Chat window##{}",uniqueID).c_str(),&m_KeepWindow);
	//m_Client->Update();

	for(auto& [address,message] : Runner.PollMessage<ChatMessage>()) {
		m_ChatMessages.emplace_back(std::bit_cast<ChatMessage>(message).ReadMessage());
	}

	for (const auto &message : m_ChatMessages)
	{
		ImGui::Text(message.c_str());
	}

	ImGui::InputText("Message", currentMessage.data(),MAX_NETMESSAGE_SIZE);
	ImGui::SameLine();

	if (const std::string message = currentMessage.data();
		ImGui::Button("Send") && !message.empty())
	{
		ChatMessage msg;
		msg.SetMessage(message);
		Runner.Send(  msg,NetworkConnection::Protocol::TCP);
		currentMessage = "";
		//m_Client->Send(message);
	}
	ImGui::End();
}
