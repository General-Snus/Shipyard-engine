#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h>
#include <Tools/Utilities/Error.hpp>

#include <filesystem>
#include <memory>
#include <unordered_set>

class Collider : public Reflectable<Collider>, public Component, SerializableTag
{
public:
	reflectable(Collider);
	Collider() = delete;
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

	template <typename T>
	std::shared_ptr<T> GetColliderAssetOfType() const;

	bool drawDebugLines = true;
	bool isTrigger = false;

	static void Serialize(StreamWriter& writer, Collider& data)
	{
		writer; data;
		throw NotImplemented();
	}

private:
	std::shared_ptr<ColliderAsset> myCollider;
};

REFL_AUTO(type(Collider), field(drawDebugLines), field(isTrigger))

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
