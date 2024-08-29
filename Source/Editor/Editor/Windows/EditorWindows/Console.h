#pragma once
#define NOMINMAX
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h"
#include "Tools/Logging/Logging.h"
#include <cstdint>

class Console : public EditorWindow
{
  public:
    void RenderImGUi() override;

  private:
    Logger::LogType filter = Logger::LogType::message;
};
