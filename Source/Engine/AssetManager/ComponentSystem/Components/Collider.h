#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h>
#include <filesystem>
#include <memory>

class cCollider : public Component
{
	friend class ColliderAsset;
public:
	cCollider() = delete; // Create a generic cube
	cCollider(const unsigned int anOwnerId); // Create a generic cube 
	cCollider(const unsigned int anOwnerId,const std::filesystem::path aPath);

	void Update() override;
	void Render() override;

	void CreateAABB(const AABB3D<float>& rf);

	template<typename T>
	std::shared_ptr<T> GetColliderAssetOfType();

private:
	std::shared_ptr<ColliderAsset> myCollider;
};

template<typename T>
inline std::shared_ptr<T> cCollider::GetColliderAssetOfType()
{
	return std::reinterpret_pointer_cast<T>(myCollider);
}
