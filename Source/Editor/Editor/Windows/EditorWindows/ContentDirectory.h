#pragma once
#include "EditorWindow.h"
#include <filesystem>

class Mesh;
class TextureHolder;
class Material;
class Scene;

class ContentDirectory : public EditorWindow
{
public:
	explicit ContentDirectory();
	void RenderImGUi() override;
private:
 	std::filesystem::path m_CurrentPath;
	std::vector<std::filesystem::path> m_CurrentDirectoryPaths;
	float cellSize = 128.f;  
	bool IsDirty = true;
};
