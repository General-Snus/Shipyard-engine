#pragma once 
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h> 
#include <Tools/Utilities/LinearAlgebra/AABB3D.hpp>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>  
#include "BaseAsset.h" 

enum class eColliderType
{
	AABB,
	RECTANGLE,
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
	MYLIB_REFLECTABLE();
	ColliderAsset() = delete; // Always tell the type stupid
	explicit ColliderAsset(eColliderType type);
	explicit ColliderAsset(const std::filesystem::path& aFilePath);
	~ColliderAsset();
	void Init() override;


	eColliderType GetColliderType() const { return type; };
	virtual void RenderDebugLines(Transform& data) = 0;
protected:
	eColliderType type;
	Sphere<float> boundingBox;
	std::vector<DebugDrawer::PrimitiveHandle> myHandles;
};
REFL_AUTO(type(ColliderAsset))


//Have thomas look at this
class ColliderAssetAABB : public ColliderAsset
{
public:
	MYLIB_REFLECTABLE();
	explicit ColliderAssetAABB();
	explicit ColliderAssetAABB(const AABB3D<float>& rf);
	void RenderDebugLines(Transform& data) override;
	void UpdateWithTransform(const Matrix& matrix);
	inline const AABB3D<float>& GetAABB() const { return myAABB; }
private:
	AABB3D<float> myAABB;
	AABB3D<float> myOriginalAABB;
};
REFL_AUTO(type(ColliderAssetAABB))

class ColliderAssetSphere : public ColliderAsset
{
public:
	MYLIB_REFLECTABLE();
	explicit ColliderAssetSphere();
	explicit ColliderAssetSphere(const Sphere<float>& rf);
	void RenderDebugLines(Transform& data) override;
private:
	Sphere<float> mySphere;
};
REFL_AUTO(type(ColliderAssetSphere))


class ColliderAssetConvex : public ColliderAsset
{
public:
	MYLIB_REFLECTABLE();
	explicit ColliderAssetConvex();
	explicit ColliderAssetConvex(const std::shared_ptr<Mesh>& rf);
	explicit ColliderAssetConvex(const std::filesystem::path& path);
	void RenderDebugLines(Transform& data) override;
	inline std::shared_ptr<Mesh> GetColliderMesh() { return aColliderMesh; }

private:
	std::shared_ptr<Mesh> aColliderMesh;
};
REFL_AUTO(type(ColliderAssetConvex))



class ColliderAssetPlanar : public ColliderAsset
{
public:

	MYLIB_REFLECTABLE();
	explicit ColliderAssetPlanar();
	explicit ColliderAssetPlanar(const std::shared_ptr<Mesh>& rf);
	explicit ColliderAssetPlanar(const std::filesystem::path& path);
	void RenderDebugLines(Transform& data) override;
	inline std::shared_ptr<Mesh> GetColliderMesh() { return aColliderMesh; }

private:
	std::shared_ptr<Mesh> aColliderMesh;
};

REFL_AUTO(type(ColliderAssetPlanar))