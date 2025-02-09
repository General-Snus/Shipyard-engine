#include "../EditorWindow.h"

#include <Editor/Editor/Commands/CommandBuffer.h>

EditorWindow::EditorWindow() : uniqueID(Math::RandomEngine::randomNumberOfType<unsigned>())
{
};

EditorWindow::~EditorWindow() = default;

void EditorWindow::Redo()
{
}

void EditorWindow::Undo()
{
}
