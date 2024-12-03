#pragma once
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/Enums.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"
#include "Tools/ImGui/ImGuiHelpers.hpp"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#define AsUINT(v) static_cast<unsigned>(v)

class Mesh;
class Material;
class TextureHolder;
class Skeleton;

class MeshRenderer : public Component
{
  public:
    ReflectableTypeRegistration();
    MeshRenderer() = delete;                                             // Create a generic cube
    MeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager); // Create a generic cube
    MeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager, const std::filesystem::path &aFilePath,
                  bool useExact = false);
    ~MeshRenderer() override = default;

    // Mesh
    void SetNewMesh(const std::filesystem::path &aFilePath);
    void SetNewMesh(const std::shared_ptr<Mesh> aMesh);
    std::vector<Element> &GetElements() const;
    std::shared_ptr<Mesh> GetRawMesh() const;
    float GetBoundingSphereRadius() const;
    AABB3D<float> GetBoundingBox() const;

    // Materials
    std::shared_ptr<Material> GetMaterial(int materialIndex = 0) const;
    void SetMaterialPath(const std::filesystem::path &aFilePath);
    void SetMaterialPath(const std::filesystem::path &aFilePath, int elementIndex);
    void SetMaterial(const std::shared_ptr<Material> aMaterial);
    void SetMaterial(const std::shared_ptr<Material> aMaterial, int elementIndex);
    std::shared_ptr<TextureHolder> GetTexture(eTextureType type, unsigned materialIndex = 0) const;

    // Other
    bool InspectorView() override;
    bool IsDefaultMesh() const;
    bool isInstanced = false;

  protected:
    std::shared_ptr<Mesh> m_Mesh;
    // Meshes can have defaulted materials but the meshrenderer has the capacity to override them.
    std::vector<std::shared_ptr<Material>>
        m_OverrideMaterial; // TODO MAKE MAP SO INSTANCE CAN KEEP ANY OVERRIDEN MATERIAL YOU MUPPET
};

REFL_AUTO(type(MeshRenderer), field(isInstanced))

class cSkeletalMeshRenderer : public MeshRenderer
{
    friend class cAnimator;

  public:
    ReflectableTypeRegistration();
    cSkeletalMeshRenderer() = delete;
    cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager);
    cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager,
                          const std::filesystem::path &aFilePath);
    ~cSkeletalMeshRenderer() override = default;

    void SetNewMesh(const std::filesystem::path &aFilePath); 
    bool InspectorView() override;

    FORCEINLINE const std::shared_ptr<Skeleton> GetRawSkeleton() const
    {
        return (mySkeleton);
    }

    std::shared_ptr<Skeleton> mySkeleton;
};

REFL_AUTO(type(cSkeletalMeshRenderer), field(mySkeleton))

void PopUpContextForAsset();
