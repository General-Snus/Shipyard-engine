#pragma once 
#include "EditorWindow.h"
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

class Hierarchy : public EditorWindow
{
public:
	void RenderImGUi() override;
private:
	void RenderNode(const Transform& transform);
};

