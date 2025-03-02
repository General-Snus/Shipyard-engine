#pragma once
#include "EditorWindow.h"
#include <array>
#include <string>
#include <vector>

class ChatWindow : public EditorWindow
{
  public:
    explicit ChatWindow();
    ~ChatWindow();
    void RenderImGUi() override;

  private:
    std::vector<std::string> m_ChatMessages;
    std::string currentMessage;
}; 