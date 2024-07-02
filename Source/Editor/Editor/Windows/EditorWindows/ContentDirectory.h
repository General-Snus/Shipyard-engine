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
	bool  GenerateSceneForIcon(std::shared_ptr<Mesh> meshAsset,std::shared_ptr<TextureHolder> renderTarget,std::shared_ptr<Material> material);
	std::filesystem::path m_CurrentPath;
	std::vector<std::filesystem::path> m_CurrentDirectoryPaths;
	float cellSize = 128.f; 
	std::shared_ptr<Scene> newScene;
	bool IsDirty = true;
};
