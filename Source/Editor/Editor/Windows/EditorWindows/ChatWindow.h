#pragma once
#include "EditorWindow.h"
#include <Networking/Client/Client/Client.h>
#include <vector>

class ChatWindow : public EditorWindow
{
  public:
    explicit ChatWindow();
    void RenderImGUi() override;

  private:
    std::vector<std::string> m_ChatMessages;
    Client m_Client;
    char currentMessage[MAX_NETMESSAGE_SIZE];
};
