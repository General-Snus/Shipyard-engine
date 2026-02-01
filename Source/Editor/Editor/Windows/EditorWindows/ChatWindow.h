#pragma once
#include "EditorWindow.h"
#include <string>
#include <vector>

class ChatWindow : public EditorWindow
{
public:
	ChatWindow() : EditorWindow("Chat Window") {}
	void Initialize() override;
	~ChatWindow();
	void RenderImGUi() override;

private:
	std::vector<std::string> m_ChatMessages;
	std::string currentMessage;
	std::string userHandle;
};
