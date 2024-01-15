#include "../ColliderAsset.h"
#include "AssetManager.pch.h"


ColliderAsset::ColliderAsset(eColliderType type) : type(type),AssetBase(L"")
{
}

ColliderAsset::ColliderAsset(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
}

ColliderAsset::~ColliderAsset()
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();

	AssetBase::~AssetBase();
}

void ColliderAsset::Init()
{
}

ColliderAssetAABB::ColliderAssetAABB() : myAABB(AABB3D<float>()),ColliderAsset(eColliderType::AABB)
{
}

ColliderAssetAABB::ColliderAssetAABB(const AABB3D<float>& rf) : myAABB(rf),ColliderAsset(eColliderType::AABB)
{
}

void ColliderAssetAABB::RenderDebugLines(const Transform& data)
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();
	//Velocity  
	DebugDrawer::PrimitiveHandle handle = DebugDrawer::Get().AddDebugBox(myAABB.GetMin(),myAABB.GetMax());
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle,data.GetTransform());
	myHandles.push_back(handle);
}

inline void ColliderAssetAABB::UpdateWithTransform(const Matrix& matrix)
{
	Vector4f minPoint = Vector4f(myOriginalAABB.GetMin(),1) * matrix;
	Vector4f maxPoint = Vector4f(myOriginalAABB.GetMax(),1) * matrix;

	const Vector3f minV3 = Vector3f(minPoint.x,minPoint.y,minPoint.z);
	const Vector3f maxV3 = Vector3f(maxPoint.x,maxPoint.y,maxPoint.z);

	myAABB = AABB3D<float>(MinVector(minV3,maxV3),MaxVector(minV3,maxV3));
}

ColliderAssetSphere::ColliderAssetSphere() : mySphere(Sphere<float>()),ColliderAsset(eColliderType::SPHERE)
{
}

ColliderAssetSphere::ColliderAssetSphere(const Sphere<float>& rf) : mySphere(rf),ColliderAsset(eColliderType::SPHERE)
{
}

void ColliderAssetSphere::RenderDebugLines(const Transform& data)
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();

	const Vector3f min = Vector3f(-1.0f,-1.0f,-1.0f).GetNormalized();
	const Vector3f max = Vector3f(1.0f,1.0f,1.0f).GetNormalized();

	//Velocity  
	DebugDrawer::PrimitiveHandle handle = DebugDrawer::Get().AddDebugBox(
		mySphere.GetCenter() + data.GetPosition() + min * mySphere.GetRadius(),
		mySphere.GetCenter() + data.GetPosition() + max * mySphere.GetRadius()
	);
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle,data.GetTransform());
	myHandles.push_back(handle);
}

ColliderAssetConvex::ColliderAssetConvex() : ColliderAsset(eColliderType::CONVEX)
{
	assert(false && "Not implemented");
}

ColliderAssetConvex::ColliderAssetConvex(const std::shared_ptr<Mesh>& rf) : ColliderAsset(eColliderType::CONVEX)
{
	rf;
	assert(false && "Not implemented");
}

ColliderAssetConvex::ColliderAssetConvex(const std::filesystem::path& aFilePath) : ColliderAsset(eColliderType::CONVEX)
{
	aFilePath;
	assert(false && "Not implemented");
}

void ColliderAssetConvex::RenderDebugLines(const Transform& data)
{
	data;
	assert(false && "Not implemented");
}
