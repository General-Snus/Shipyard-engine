#pragma once
#include "EditorWindow.h"
#include <Editor/Editor/Core/ApplicationState.h>
#include <array> // Ensure the array header is included

class GraphicsDebugger : public EditorWindow
{
public:
	GraphicsDebugger() : EditorWindow("GraphicsDebugger") {}
	void RenderImGUi() override;

private:
	int currentlyActiveTone = 0;
	int currentlyActiveLayer = 0;
};
