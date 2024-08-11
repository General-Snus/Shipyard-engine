#pragma once
#define NOMINMAX
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h"
#include <cstdint>
#include "Tools/Logging/Logging.h"

class Console : public EditorWindow
{
public:
	void RenderImGUi() override;
private:
	Logger::LogType filter = Logger::LogType::All;
};

