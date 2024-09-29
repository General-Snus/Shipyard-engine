#pragma once
#include "BaseAsset.h"
#include "Tools/Utilities/LinearAlgebra/AABB3D.hpp"
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>

class Material;
class TextureHolder;
struct aiMesh;
struct aiScene;
class Mesh : public AssetBase
{
  public:
    MYLIB_REFLECTABLE();
    friend class MeshRenderer;
    friend class InstanceRenderer;
    Mesh() = default;
    Mesh(const std::filesystem::path &aFilePath);
    void Init() override;

    AABB3D<float> Bounds;
    std::vector<Element> Elements;
    const std::unordered_map<unsigned int, std::shared_ptr<Material>> &GetMaterialList();
    void FillMaterialPaths(const aiScene *scene);
    bool InspectorView() override;
    std::shared_ptr<TextureHolder> GetEditorIcon() override;

  private:
    std::unordered_map<unsigned int, std::shared_ptr<Material>> materials;
    std::unordered_map<unsigned int, std::filesystem::path> idToMaterial;

    void processMesh(aiMesh *mesh, const aiScene *scene);
    void ResizeBuffer();
    void UpdateInstanceBuffer();
};

REFL_AUTO(type(Mesh, bases<AssetBase>), field(Bounds))
