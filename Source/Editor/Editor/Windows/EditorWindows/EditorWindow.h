#pragma once
#include <memory>

class CommandBuffer;

class EditorWindow
{
public:
	EditorWindow() = default;
	virtual ~EditorWindow() = default;
	virtual void RenderImGUi() = 0;

	void Redo();
	void Undo();
	bool m_KeepWindow = true;

private:
	//Why pointer, because you want to store them somewhere else because edit history 
	//should not be lost after closing and reopening a context
	std::shared_ptr<CommandBuffer> m_CommandBuffer;
};

