#pragma once
#include "EditorWindow.h"
#include <vector>
#include <string>
class Client;
class ChatWindow : public EditorWindow
{
  public:
    explicit ChatWindow();
    ~ChatWindow();
    void RenderImGUi() override;

  private:
    std::vector<std::string> m_ChatMessages;
    Client* m_Client;
    char currentMessage[512];
}; 