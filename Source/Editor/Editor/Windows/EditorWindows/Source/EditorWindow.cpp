#include "../EditorWindow.h" 
#include "Tools\Utilities\Math.hpp"
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Tools/ImGui/imgui.h>

EditorWindow::EditorWindow(const char* name, int flags) :
	name(name),
	uniqueID(Math::RandomEngine::randomNumberOfType<unsigned>()),
	flags((ImGuiWindowFlags_)flags)
{
};

EditorWindow::~EditorWindow() = default;

void EditorWindow::Redo()
{
}

void EditorWindow::Undo()
{
}
