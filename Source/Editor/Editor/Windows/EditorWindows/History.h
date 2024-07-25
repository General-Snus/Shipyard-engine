#pragma once
#define NOMINMAX
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h" 
#include "Tools/Logging/Logging.h"

class History : public EditorWindow
{
public:
	void RenderImGUi() override; 
};

