#include "../ChatWindow.h"

#include "Tools/Utilities/Input/Input.hpp"
#include "Windows/Window.h"
#include "imgui.h" 
#include <Engine/PersistentSystems/Networking/NetMessage/StringMessages.h> 
#include <Engine/PersistentSystems/Networking/NetworkRunner.h>

void ChatWindow::Initialize()
{
	currentMessage.resize(512);
	userHandle.resize(32);
	userHandle = WindowInstance.GetWindowsTitle();
}

ChatWindow::~ChatWindow() = default;

void ChatWindow::RenderImGUi()
{
	if (ImGui::InputText("Nickname:", userHandle.data(), 32))
	{
		const std::string message = userHandle.data(); // String wont register size diff  and size will be zero if we dont do this
		NicknameMessage msg;
		msg.SetMessage(message);
		Runner.Send(msg, NetworkConnection::Protocol::TCP);
	}

	bool newMessage = false;
	for (const auto& message : Runner.PollMessage<ChatMessage>())
	{
		m_ChatMessages.emplace_back(std::bit_cast<ChatMessage>(message.message).ReadMessage());
		newMessage = true;
	}

	ImGui::BeginChild("MessageFrame");
	for (const auto& message : m_ChatMessages)
	{
		ImGui::Text(message.c_str());
	}

	if (newMessage && ImGui::GetScrollY() > .75f)
	{
		ImGui::SetScrollHereY(1.0);
	}

	ImGui::EndChild();

	ImGui::InputTextMultiline("Message", currentMessage.data(), MAX_NETMESSAGE_SIZE);
	ImGui::SameLine();

	if (const std::string message = currentMessage.data();
		(ImGui::Button("Send") || Input.IsKeyPressed(Keys::RETURN)) && !message.empty())
	{
		ImGui::SetKeyboardFocusHere(-1);
		ChatMessage msg;
		msg.SetMessage(message);
		Runner.Send(msg, NetworkConnection::Protocol::TCP);
		currentMessage = "";
	}
}
