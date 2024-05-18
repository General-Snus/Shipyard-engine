#pragma once

class EditorWindow
{
public:
	EditorWindow() = default;
	virtual ~EditorWindow() = default;
	virtual void RenderImGUi() = 0;
};

