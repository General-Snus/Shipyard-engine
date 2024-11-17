#pragma once
#include "EditorWindow.h"
#include <filesystem>

class Mesh;
class TextureHolder;
class Material;
class Scene;
struct pathData
{
	std::string name;
	std::filesystem::path fullPath;
	bool isDirectory;
};
class ContentDirectory : public EditorWindow
{
  public:
    explicit ContentDirectory();
    void Node(const int index, const float cellWidth);
    void RenderImGUi() override;

    void FillPaths(const std::string &keyTerm);

    void DoMoveOperation();

    void WMDroppedFile();

  private:
    std::shared_ptr<TextureHolder> IconFromExtension(const std::filesystem::path &extension,
                                                     const std::filesystem::path &path, bool isDirectory);
    void DirectoryBar();

    std::filesystem::path m_CurrentPath; 

    std::vector<pathData> m_CurrentDirectoryPaths;
    float cellSize = 128.f;
    bool IsDirty = true;

    bool isMoveOperationUnderway = false;
    bool isLookingForHovered = false;
    std::filesystem::path m_hoveredPath;
    char buf[128] = "";
};
