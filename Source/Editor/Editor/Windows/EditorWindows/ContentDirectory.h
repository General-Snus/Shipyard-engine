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
    void Node(const int index, const float cellWidth);
    void RenderImGUi() override;

    void FillPaths(const std::string &keyTerm);

    void DoMoveOperation();

    void WMDroppedFile();

  private:
    std::shared_ptr<TextureHolder> IconFromExtension(const std::filesystem::path &fullPath,
                                                     const std::filesystem::path &extension,
                                                     const std::filesystem::path &path);
    void DirectoryBar();

    std::filesystem::path m_CurrentPath;
    using pathPair = std::pair<std::string, std::filesystem::path>;
    std::vector<pathPair> m_CurrentDirectoryPaths;
    float cellSize = 128.f;
    bool IsDirty = true;

    bool isMoveOperationUnderway = false;
    bool isLookingForHovered = false;
    std::filesystem::path m_hoveredPath;
    char buf[128] = "";
};
