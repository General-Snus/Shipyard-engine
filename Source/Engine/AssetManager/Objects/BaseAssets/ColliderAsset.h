#pragma once
#include "Engine/AssetManager/Objects/BaseAssets/BaseAsset.h"
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Tools/Utilities/LinearAlgebra/AABB3D.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>

enum class eColliderType
{
    AABB,
    BOX,
    SPHERE,
    CIRCLE,
    PLANE,
    CONE,
    FRUSTRUM,
    TRIANGLE,
    RAY,
    CONVEX,
    PLANAR
};

class Transform;
class DebugDrawer;
class Mesh;
struct PrimitiveHandle;

class ColliderAsset : public AssetBase
{
  public:
    ReflectableTypeRegistration();
    ColliderAsset() = delete; // Always tell the type stupid
    explicit ColliderAsset(eColliderType type);
    explicit ColliderAsset(const std::filesystem::path &aFilePath);
    ~ColliderAsset();
    void Init() override;

    eColliderType GetColliderType() const
    {
        return type;
    };
    virtual void RenderDebugLines(Transform &data) = 0;
    void RemoveDebugLines();
    bool InspectorView() override
    {
        auto fl = float(boundingBox.GetRadius());
        ImGui::Text(std::format("Collider Type: {}", magic_enum::enum_name(type)).c_str());
        ImGui::DragFloat("Bounding Box Radius", &fl);
        return false;
    }
    virtual void UpdateWithTransform(const Matrix& matrix) { matrix; };

  protected:
      bool shouldRemoveLines = false;
    eColliderType type;
    Sphere<float> boundingBox;
    std::vector<DebugDrawer::PrimitiveHandle> myHandles;
};
REFL_AUTO(type(ColliderAsset))

// Have thomas look at this
class ColliderAssetAABB : public ColliderAsset
{
  public:
    ReflectableTypeRegistration();
    explicit ColliderAssetAABB();
    explicit ColliderAssetAABB(const AABB3D<float> &rf);
    void RenderDebugLines(Transform &data) override;
    void UpdateWithTransform(const Matrix &matrix) override;
    inline const AABB3D<float> &ScaledAABB() const
    {
        return myAABB;
    }

    inline AABB3D<float>& BaseAABB() {
        return myOriginalAABB;
    }

    bool InspectorView() override
    {
        ColliderAsset::InspectorView();
        auto& center = myOriginalAABB.GetCenter();
        auto& extent = myOriginalAABB.GetExtent();
        ImGui::DragFloat3("center",&center.x);
        ImGui::DragFloat3("box extent",&extent.x);
        return false;
    }

  private:
    AABB3D<float> myAABB;
    AABB3D<float> myOriginalAABB;
};
REFL_AUTO(type(ColliderAssetAABB))

class ColliderAssetBox : public ColliderAsset {
public:
    ReflectableTypeRegistration();
    explicit ColliderAssetBox();
    explicit ColliderAssetBox(const AABB3D<float>& rf);
    void RenderDebugLines(Transform& data) override; 

    AABB3D<float>& box() { return myBox; };

    bool InspectorView() override {
        ColliderAsset::InspectorView();
        auto& center = myBox.GetCenter();
        auto& extent = myBox.GetExtent();
        ImGui::DragFloat3("center",&center.x);
        ImGui::DragFloat3("box extent",&extent.x);
        return false;
    }
private:
    AABB3D<float> myBox;
};
REFL_AUTO(type(ColliderAssetBox))

class ColliderAssetSphere : public ColliderAsset
{
  public:
    ReflectableTypeRegistration();
    explicit ColliderAssetSphere();
    explicit ColliderAssetSphere(const Sphere<float> &rf);
    void RenderDebugLines(Transform &data) override;
    Sphere<float>& sphere() { return mySphere; };


  private:
    Sphere<float> mySphere;
};
REFL_AUTO(type(ColliderAssetSphere))

class ColliderAssetConvex : public ColliderAsset
{
  public:
    ReflectableTypeRegistration();
    explicit ColliderAssetConvex();
    explicit ColliderAssetConvex(const std::shared_ptr<Mesh> &rf);
    explicit ColliderAssetConvex(const std::filesystem::path &path);
    void RenderDebugLines(Transform &data) override;
    inline std::shared_ptr<Mesh> GetColliderMesh()
    {
        return aColliderMesh;
    }

  private:
    std::shared_ptr<Mesh> aColliderMesh;
};
REFL_AUTO(type(ColliderAssetConvex))

class ColliderAssetPlanar : public ColliderAsset
{
  public:
    ReflectableTypeRegistration();
    explicit ColliderAssetPlanar();
    explicit ColliderAssetPlanar(const std::shared_ptr<Mesh> &rf);
    explicit ColliderAssetPlanar(const std::filesystem::path &path);
    void RenderDebugLines(Transform &data) override;
    inline std::shared_ptr<Mesh> GetColliderMesh()
    {
        return aColliderMesh;
    }

  private:
    std::shared_ptr<Mesh> aColliderMesh;
};

REFL_AUTO(type(ColliderAssetPlanar))
