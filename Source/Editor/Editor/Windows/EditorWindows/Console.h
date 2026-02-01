#pragma once
#define NOMINMAX
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h"
#include "Tools/Logging/Logging.h"
#include <cstdint>

class Console : public EditorWindow
{
public:
	Console() : EditorWindow("Console", ImGuiWindowFlags_NoResize) {};
	void RenderImGUi() override;
	void PushDebugLayerToConsole();

private:
	LoggerService::LogType filter = LoggerService::LogType::message;
	LoggerService::LogMsg pickedMessage;
	std::vector<LoggerService::LogMsg> clippedMessages;
};
