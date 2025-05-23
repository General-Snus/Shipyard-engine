#pragma once
#include "BaseAsset.h"
#include "Tools/Utilities/LinearAlgebra/AABB3D.hpp"
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include "DirectX/DX12/Graphics/Resources/IndexBuffer.h"
#include "DirectX/DX12/Graphics/Resources/VertexBuffer.h"

class Material; 
class TextureHolder;
struct aiMesh;
struct aiScene;

struct Element
{
	VertexResource VertexBuffer; // remove and replace with offset
	IndexResource IndexResource;
	AABB3D<float> Bounds;
	UINT Stride = 0;
	unsigned int MaterialIndex = 0;
};

class Mesh : public AssetBase
{
  public:
    ReflectableTypeRegistration();
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

    bool processMesh(aiMesh *mesh, const aiScene *scene, Element& outElement);
    void ResizeBuffer();
    void UpdateInstanceBuffer();
};

REFL_AUTO(type(Mesh, bases<AssetBase>), field(Bounds))
