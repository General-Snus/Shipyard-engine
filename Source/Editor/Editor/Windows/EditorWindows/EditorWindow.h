#pragma once

class EditorWindow
{
public:
	EditorWindow() = default;
	virtual ~EditorWindow() = default;
	virtual void RenderImGUi() = 0;
	bool m_KeepWindow = true;
};

