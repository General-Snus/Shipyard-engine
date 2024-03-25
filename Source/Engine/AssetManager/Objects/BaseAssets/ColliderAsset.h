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


//Have thomas look at this
class ColliderAssetAABB : public ColliderAsset
{
public:
	explicit ColliderAssetAABB();
	explicit ColliderAssetAABB(const AABB3D<float>& rf);
	void RenderDebugLines(Transform& data) override;
	void UpdateWithTransform(const Matrix& matrix);
	inline const AABB3D<float>& GetAABB() const { return myAABB; }
private:
	AABB3D<float> myAABB;
	AABB3D<float> myOriginalAABB;
};

class ColliderAssetSphere : public ColliderAsset
{
public:
	explicit ColliderAssetSphere();
	explicit ColliderAssetSphere(const Sphere<float>& rf);
	void RenderDebugLines(Transform& data) override;
private:
	Sphere<float> mySphere;
};


class ColliderAssetConvex : public ColliderAsset
{
public:
	explicit ColliderAssetConvex();
	explicit ColliderAssetConvex(const std::shared_ptr<Mesh>& rf);
	explicit ColliderAssetConvex(const std::filesystem::path& path);
	void RenderDebugLines(Transform& data) override;
	inline std::shared_ptr<Mesh> GetColliderMesh() { return aColliderMesh; }

private:
	std::shared_ptr<Mesh> aColliderMesh;
};



class ColliderAssetPlanar : public ColliderAsset
{
public:
	explicit ColliderAssetPlanar();
	explicit ColliderAssetPlanar(const std::shared_ptr<Mesh>& rf);
	explicit ColliderAssetPlanar(const std::filesystem::path& path);
	void RenderDebugLines(Transform& data) override;
	inline std::shared_ptr<Mesh> GetColliderMesh() { return aColliderMesh; }

private:
	std::shared_ptr<Mesh> aColliderMesh;
};