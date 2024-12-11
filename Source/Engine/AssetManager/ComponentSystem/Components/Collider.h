#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h>

#include <filesystem>
#include <memory>
#include <unordered_set>

class Collider : public Component
{
	friend class ColliderAsset;

public:
	ReflectableTypeRegistration();
	Collider() = delete; // Create a generic cube
	Collider(SY::UUID anOwnerId, GameObjectManager* aManager);
	Collider(SY::UUID anOwnerId, GameObjectManager* aManager, const std::filesystem::path& aPath);
	void     Update() override;
	Vector3f GetClosestPosition(Vector3f position) const;
	Vector3f GetNormalToward(Vector3f position) const;
	void     Destroy() override;

	eColliderType GetColliderType() const
	{
		return myCollider->GetColliderType();
	}

	template <typename T, typename... Args>
	void SetColliderType(Args... someParameters);
	void OnSiblingChanged(const std::type_info* SourceClass = nullptr) override;
	bool InspectorView() override;

	// void AddToNotify(std::weak_ptr<Component> aComponent) { myNotify.push_back(aComponent); }
	// void Notify(std::weak_ptr<Collider> notifier) { for(auto& i : myNotify) i.lock()->CollidedWith(notifier); }

	template <typename T> // add inheritance check here when not lazy stupid
	std::shared_ptr<T> GetColliderAssetOfType() const;

	bool drawDebugLines = false;

private:
	//std::unordered_set<SY::UUID> currentlyCollidingObjects;
	std::shared_ptr<ColliderAsset> myCollider;
	// std::vector<std::weak_ptr<Component>> myNotify;
};

REFL_AUTO(type(Collider), field(drawDebugLines))

template <typename T, typename... Args>
void Collider::SetColliderType(Args... someParameters)
{
	myCollider = std::make_shared<T>(someParameters...);
}

template <typename T>
std::shared_ptr<T> Collider::GetColliderAssetOfType() const
{
	return std::reinterpret_pointer_cast<T>(myCollider);
}
