#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
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
	Vector3f GetClosestPosition(Vector3f position) const;
	Vector3f GetNormalToward(Vector3f position) const;
	void Render() override;

	eColliderType GetColliderType() const { return myCollider->GetColliderType(); }

	void CreateAABB(const AABB3D<float>& rf);

	//void AddToNotify(std::weak_ptr<Component> aComponent) { myNotify.push_back(aComponent); }
	//void Notify(std::weak_ptr<cCollider> notifier) { for(auto& i : myNotify) i.lock()->CollidedWith(notifier); }



	template<typename T> // add inheritance check here when not lazy stupid
	std::shared_ptr<T> GetColliderAssetOfType() const;

private:
	std::shared_ptr<ColliderAsset> myCollider;
	//std::vector<std::weak_ptr<Component>> myNotify;
};

template<typename T>
inline std::shared_ptr<T> cCollider::GetColliderAssetOfType() const
{
	return std::reinterpret_pointer_cast<T>(myCollider);
}

