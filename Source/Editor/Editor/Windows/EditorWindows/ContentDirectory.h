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

    void WMDroppedFile();

  private:
    std::shared_ptr<TextureHolder> IconFromExtension(const std::filesystem::path &fullPath,
                                                     const std::filesystem::path &extension,
                                                     const std::filesystem::path &path);
    void DirectoryBar();

    std::filesystem::path m_CurrentPath;
    std::vector<std::filesystem::path> m_CurrentDirectoryPaths;
    float cellSize = 128.f;
    bool IsDirty = true;

    bool isMoveOperationUnderway = false;
    bool isLookingForHovered = false;
    std::filesystem::path m_hoveredPath;
};
