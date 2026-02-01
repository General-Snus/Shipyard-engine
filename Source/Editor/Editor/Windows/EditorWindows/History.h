#pragma once
#define NOMINMAX
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h" 
#include "Tools/Logging/Logging.h"

class History : public EditorWindow
{
public:
	History() : EditorWindow("History") {}
	void RenderImGUi() override;
};

