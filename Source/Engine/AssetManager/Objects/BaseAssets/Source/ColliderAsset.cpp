#include "AssetManager.pch.h"
#include "Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"

#include <Tools/ImGui/ImGuizmo.h>
#include <Tools/Utilities/LinearAlgebra/AABB3D.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

ColliderAsset::ColliderAsset(eColliderType type) : AssetBase(L""), type(type)
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

ColliderAssetAABB::ColliderAssetAABB() : ColliderAsset(eColliderType::AABB)
{
}

ColliderAssetAABB::ColliderAssetAABB(const AABB3D<float>& rf) : ColliderAsset(eColliderType::AABB), myAABB(rf)
{
}

void ColliderAssetAABB::RenderDebugLines(Transform& data)
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();
	// Velocity
	const DebugDrawer::PrimitiveHandle handle = DebugDrawer::Get().AddDebugBox(myAABB.GetMin(), myAABB.GetMax());
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle, data.GetTransform());
	myHandles.push_back(handle);
}

void ColliderAssetAABB::UpdateWithTransform(const Matrix& matrix)
{
	const Vector4f minPoint = Vector4f(myOriginalAABB.GetMin() * .5f, 1) * matrix;
	const Vector4f maxPoint = Vector4f(myOriginalAABB.GetMax() * .5f, 1) * matrix;

	const auto minV3 = Vector3f(minPoint.x, minPoint.y, minPoint.z);
	const auto maxV3 = Vector3f(maxPoint.x, maxPoint.y, maxPoint.z);

	myAABB = AABB3D(MinVector(minV3, maxV3), MaxVector(minV3, maxV3));
}

ColliderAssetSphere::ColliderAssetSphere() : ColliderAsset(eColliderType::SPHERE), mySphere(Sphere<float>())
{
}

ColliderAssetSphere::ColliderAssetSphere(const Sphere<float>& rf) : ColliderAsset(eColliderType::SPHERE), mySphere(rf)
{
}

void ColliderAssetSphere::RenderDebugLines(Transform& data)
{
	for (DebugDrawer::PrimitiveHandle& handle : myHandles)
	{
		DebugDrawer::Get().RemoveDebugPrimitive(handle);
	}

	myHandles.clear();

	const Vector3f min = Vector3f(-1.0f, -1.0f, -1.0f).GetNormalized();
	const Vector3f max = Vector3f(1.0f, 1.0f, 1.0f).GetNormalized();

	// Velocity
	const DebugDrawer::PrimitiveHandle handle =
		DebugDrawer::Get().AddDebugBox(mySphere.GetCenter() + data.GetPosition() + min * mySphere.GetRadius(),
		                               mySphere.GetCenter() + data.GetPosition() + max * mySphere.GetRadius());
	DebugDrawer::Get().SetDebugPrimitiveTransform(handle, data.GetTransform());
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

void ColliderAssetConvex::RenderDebugLines(Transform& data)
{
	data;
	assert(false && "Not implemented");
}

ColliderAssetPlanar::ColliderAssetPlanar() : ColliderAsset(eColliderType::PLANAR)
{
}

ColliderAssetPlanar::ColliderAssetPlanar(const std::shared_ptr<Mesh>& rf) : ColliderAsset(eColliderType::PLANAR)
{
	aColliderMesh = rf;
}

ColliderAssetPlanar::ColliderAssetPlanar(const std::filesystem::path& path) : ColliderAsset(eColliderType::PLANAR)
{
	ENGINE_RESOURCES.ForceLoadAsset<Mesh>(path, aColliderMesh);
	path;
	// assert(false && "Not implemented");
}

void ColliderAssetPlanar::RenderDebugLines(Transform& data)
{
	data;
	assert(false && "Not implemented");
}
