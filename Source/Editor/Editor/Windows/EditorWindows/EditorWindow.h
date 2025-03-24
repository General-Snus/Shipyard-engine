#pragma once
#include <memory>

class CommandBuffer;

class EditorWindow
{
public:
	EditorWindow();
	virtual      ~EditorWindow();
	virtual void RenderImGUi() = 0;

	void Redo();
	void Undo();
	bool m_KeepWindow = true;

	const unsigned uniqueID; 
private:
	//Why pointer, because you want to store them somewhere else because edit history 
	//should not be lost after closing and reopening a context
	std::shared_ptr<CommandBuffer> m_CommandBuffer;
};
