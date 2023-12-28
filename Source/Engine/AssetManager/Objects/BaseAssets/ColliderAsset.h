#pragma once
#include "BaseAsset.h" 
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Tools/Utilities/LinearAlgebra/AABB3D.hpp>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>  

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
	CONVEX
};

class Transform;
class DebugDrawer;
struct PrimitiveHandle;


class ColliderAsset : public AssetBase
{
public:
	ColliderAsset() = delete; // Always tell the type stupid
	explicit ColliderAsset(eColliderType type);
	explicit ColliderAsset(const std::filesystem::path& aFilePath);
	void Init() override;


	eColliderType GetColliderType() const { return type; };
	virtual void RenderDebugLines(const Transform& data) = 0;
protected:
	eColliderType type;
	Sphere<float> boundingBox;
	std::vector<DebugDrawer::PrimitiveHandle> myHandles;
};

class ColliderAssetAABB : public ColliderAsset
{
public:
	explicit ColliderAssetAABB();
	explicit ColliderAssetAABB(const AABB3D<float>& rf);
	void RenderDebugLines(const Transform& data) override;
	inline const AABB3D<float>& GetAABB() const { return myAABB; }
private:
	AABB3D<float> myAABB;
};

class ColliderAssetSphere : public ColliderAsset
{
public:
	explicit ColliderAssetSphere();
	explicit ColliderAssetSphere(const Sphere<float>& rf);
	void RenderDebugLines(const Transform& data) override;
private:
	Sphere<float> mySphere;
};