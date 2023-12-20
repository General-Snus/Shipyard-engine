#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/ColliderAsset.h>
#include <filesystem>

class cCollider : public Component
{
	friend class ColliderAsset;
public:
	cCollider() = delete; // Create a generic cube
	cCollider(const unsigned int anOwnerId); // Create a generic cube 
	cCollider(const unsigned int anOwnerId,const std::filesystem::path aPath);

	void Update() override;
	void Render() override;

private:
	std::shared_ptr<ColliderAsset> myCollider;
};
