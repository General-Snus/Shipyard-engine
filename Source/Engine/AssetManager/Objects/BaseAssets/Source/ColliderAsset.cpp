#include "AssetManager.pch.h"

#include "Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"

#include <Tools/ImGui/ImGuizmo.h>
#include <Tools/Utilities/LinearAlgebra/AABB3D.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

ColliderAsset::ColliderAsset(eColliderType type) : AssetBase(L""),type(type) {}

ColliderAsset::ColliderAsset(const std::filesystem::path& aFilePath) : AssetBase(aFilePath) {}

ColliderAsset::~ColliderAsset() {
	for(DebugDrawer::PrimitiveHandle& handle : myHandles) {
		GraphicsEngineInstance.debugDrawer.RemoveDebugPrimitive(handle);
	}

	myHandles.clear();

	AssetBase::~AssetBase();
}

void ColliderAsset::Init() {}

void ColliderAsset::RemoveDebugLines() {
	for(DebugDrawer::PrimitiveHandle& handle : myHandles) {
		GraphicsEngineInstance.debugDrawer.RemoveDebugPrimitive(handle);
	}
}

ColliderAssetAABB::ColliderAssetAABB() : ColliderAsset(eColliderType::AABB) {}

ColliderAssetAABB::ColliderAssetAABB(const AABB3D<float>& rf) : ColliderAsset(eColliderType::AABB),myAABB(rf) {}

void ColliderAssetAABB::RenderDebugLines(Transform& data) {
	for(DebugDrawer::PrimitiveHandle& handle : myHandles) {
		GraphicsEngineInstance.debugDrawer.RemoveDebugPrimitive(handle);
	}
	myHandles.clear();
	const auto handle =
		GraphicsEngineInstance.debugDrawer.AddDebugBox(myOriginalAABB.GetMin(),myOriginalAABB.GetMax());
	myHandles.push_back(handle);

	for(auto const& aHandle : myHandles) {
		GraphicsEngineInstance.debugDrawer.SetDebugPrimitiveTransform(aHandle,data.WorldMatrix());
	}
}

void ColliderAssetAABB::UpdateWithTransform(const Matrix& matrix) {
	const Vector4f minPoint = Vector4f(myOriginalAABB.GetMin(),1) * matrix;
	const Vector4f maxPoint = Vector4f(myOriginalAABB.GetMax(),1) * matrix;

	const auto minV3 = minPoint.xyz();
	const auto maxV3 = maxPoint.xyz();

	myAABB = AABB3D(MinVector(minV3,maxV3),MaxVector(minV3,maxV3));
}

ColliderAssetSphere::ColliderAssetSphere() : ColliderAsset(eColliderType::SPHERE),mySphere(Sphere<float>()) {}

ColliderAssetSphere::ColliderAssetSphere(const Sphere<float>& rf) : ColliderAsset(eColliderType::SPHERE),mySphere(rf) {}

void ColliderAssetSphere::RenderDebugLines(Transform& data) {
	for(DebugDrawer::PrimitiveHandle& handle : myHandles) {
		GraphicsEngineInstance.debugDrawer.RemoveDebugPrimitive(handle);
	}

	myHandles.clear();

	const Vector3f min = Vector3f(-1.0f,-1.0f,-1.0f);
	const Vector3f max = Vector3f(1.0f,1.0f,1.0f);
	 
	const DebugDrawer::PrimitiveHandle handle = GraphicsEngineInstance.debugDrawer.AddDebugBox(
		min * mySphere.GetRadius(),
		max * mySphere.GetRadius());
	GraphicsEngineInstance.debugDrawer.SetDebugPrimitiveTransform(handle,data.LocalMatrix());
	myHandles.push_back(handle);
}

ColliderAssetConvex::ColliderAssetConvex() : ColliderAsset(eColliderType::CONVEX) {
	assert(false && "Not implemented");
}

ColliderAssetConvex::ColliderAssetConvex(const std::shared_ptr<Mesh>& rf) : ColliderAsset(eColliderType::CONVEX) {
	rf;
	assert(false && "Not implemented");
}

ColliderAssetConvex::ColliderAssetConvex(const std::filesystem::path& aFilePath) : ColliderAsset(eColliderType::CONVEX) {
	aFilePath;
	assert(false && "Not implemented");
}

void ColliderAssetConvex::RenderDebugLines(Transform& data) {
	data;
	assert(false && "Not implemented");
}

ColliderAssetPlanar::ColliderAssetPlanar() : ColliderAsset(eColliderType::PLANAR) {}

ColliderAssetPlanar::ColliderAssetPlanar(const std::shared_ptr<Mesh>& rf) : ColliderAsset(eColliderType::PLANAR) {
	aColliderMesh = rf;
}

ColliderAssetPlanar::ColliderAssetPlanar(const std::filesystem::path& path) : ColliderAsset(eColliderType::PLANAR) {
	path;
	ENGINE_RESOURCES.ForceLoadAsset<Mesh>(path,aColliderMesh);
}

void ColliderAssetPlanar::RenderDebugLines(Transform& data) {
	data;
	assert(false && "Not implemented");
}

ColliderAssetBox::ColliderAssetBox() : ColliderAsset(eColliderType::BOX) {};

ColliderAssetBox::ColliderAssetBox(const AABB3D<float>& rf) : ColliderAsset(eColliderType::BOX),myBox(rf) {}

void ColliderAssetBox::RenderDebugLines(Transform& data) {
	for(DebugDrawer::PrimitiveHandle& handle : myHandles) {
		GraphicsEngineInstance.debugDrawer.RemoveDebugPrimitive(handle);
	}
	myHandles.clear();
	const auto handle =
		GraphicsEngineInstance.debugDrawer.AddDebugBox(myBox.GetMin(),myBox.GetMax());
	myHandles.push_back(handle);

	for(auto const& aHandle : myHandles) {
		GraphicsEngineInstance.debugDrawer.SetDebugPrimitiveTransform(aHandle,data.WorldMatrix());
	}
}

