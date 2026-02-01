#pragma once 
#include "EditorWindow.h"
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

class Hierarchy : public EditorWindow
{
public:
	Hierarchy() : EditorWindow("Hierarchy") {}
	void RenderImGUi() override;
private:
	void PopupMenu(SY::UUID id);
	void RenderNode(Transform& transform);
	void DragDrop(Transform& transform);
};

