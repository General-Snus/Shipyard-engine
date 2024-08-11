#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h>
#include <filesystem>
#include <memory>

class cCollider : public Component
{
	friend class ColliderAsset;
public:
	MYLIB_REFLECTABLE();
	cCollider() = delete; // Create a generic cube
	cCollider(const SY::UUID anOwnerId,GameObjectManager* aManager);
	cCollider(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aPath);
	void Update() override;
	Vector3f GetClosestPosition(Vector3f position) const;
	Vector3f GetNormalToward(Vector3f position) const;
	void Render() override;

	eColliderType GetColliderType() const { return myCollider->GetColliderType(); }

	template<typename T,typename... Args>
	void SetColliderType(Args... someParameters);

	void OnSiblingChanged(const std::type_info* SourceClass = nullptr) override;
	bool InspectorView() override;

	//void AddToNotify(std::weak_ptr<Component> aComponent) { myNotify.push_back(aComponent); }
	//void Notify(std::weak_ptr<cCollider> notifier) { for(auto& i : myNotify) i.lock()->CollidedWith(notifier); }



	template<typename T> // add inheritance check here when not lazy stupid
	std::shared_ptr<T> GetColliderAssetOfType() const;

private:
	std::shared_ptr<ColliderAsset> myCollider;
	//std::vector<std::weak_ptr<Component>> myNotify;
};
REFL_AUTO(type(cCollider))

template<typename T,typename... Args>
inline void cCollider::SetColliderType(Args... someParameters)
{
	myCollider = std::make_shared<T>(someParameters ...);
}

template<typename T>
inline std::shared_ptr<T> cCollider::GetColliderAssetOfType() const
{
	return std::reinterpret_pointer_cast<T>(myCollider);
}

