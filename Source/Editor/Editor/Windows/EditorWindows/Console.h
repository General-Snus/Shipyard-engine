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
    LoggerService::LogType filter = LoggerService::LogType::message;
    LoggerService::LogMsg pickedMessage;
};
