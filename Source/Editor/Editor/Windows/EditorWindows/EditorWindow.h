#pragma once
#include "Tools\ImGui\imgui.h"
#include <memory>

class CommandBuffer;

class EditorWindow
{
public:
	EditorWindow(const char* name, int flags = 0);
	EditorWindow() = delete;
	virtual void Initialize() {};
	virtual ~EditorWindow() noexcept;
	virtual void RenderImGUi() = 0;

	void Redo();
	void Undo();
	bool m_KeepWindow = true;

	const ImGuiWindowFlags_ flags;
	const char* name;
	const unsigned uniqueID;
private:
	//Why pointer, because you want to store them somewhere else because edit history 
	//should not be lost after closing and reopening a context
	std::shared_ptr<CommandBuffer> m_CommandBuffer;
};
