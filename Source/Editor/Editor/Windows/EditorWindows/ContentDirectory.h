#pragma once
#include "EditorWindow.h"
#include <filesystem>

class ContentDirectory : public EditorWindow
{
public:
	explicit ContentDirectory();
	void RenderImGUi() override;
private:
	std::filesystem::path m_CurrentPath;
	float cellSize = 128.f;
};
