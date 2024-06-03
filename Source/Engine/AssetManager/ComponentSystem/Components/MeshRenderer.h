#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/Enums.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"
#define AsUINT(v) static_cast<unsigned>(v)

class Mesh;
class Material;
class Skeleton;

class cMeshRenderer : public Component
{
public:
	MYLIB_REFLECTABLE();
	cMeshRenderer() = delete; // Create a generic cube
	cMeshRenderer(const unsigned int anOwnerId); // Create a generic cube  
	cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath,bool useExact = false);
	void Render() override;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath,int elementIndex);
	~cMeshRenderer() override = default;

	std::vector<Element>& GetElements() const;
	std::shared_ptr<Mesh> GetRawMesh() const;
	void InspectorView() override;

	std::shared_ptr<TextureHolder> GetTexture(eTextureType type,unsigned materialIndex = 0) const;
	bool IsDefaultMesh() const;
	bool isInstanced = false;

	std::shared_ptr<Mesh> m_Mesh;
	std::vector<std::shared_ptr<Material>> m_OverrideMaterial;
};

REFL_AUTO(
	type(cMeshRenderer),
	field(isInstanced),
	field(m_Mesh),
	field(m_OverrideMaterial)
)

class cSkeletalMeshRenderer : public cMeshRenderer
{
	friend class cAnimator;
public:
	cSkeletalMeshRenderer() = delete;
	cSkeletalMeshRenderer(const unsigned int anOwnerId);
	cSkeletalMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	~cSkeletalMeshRenderer() override = default;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void Render() override;
	void InspectorView() override;

	FORCEINLINE const std::shared_ptr<Skeleton> GetRawSkeleton() const
	{
		return (mySkeleton);
	}

	std::shared_ptr<Skeleton> mySkeleton;
};

REFL_AUTO(
	type(cSkeletalMeshRenderer),
	field(mySkeleton)
)