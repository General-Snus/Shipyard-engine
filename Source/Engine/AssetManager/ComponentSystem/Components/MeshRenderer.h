#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/Enums.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"
#define AsUINT(v) static_cast<unsigned>(v)

class Mesh;
class Material;
class TextureHolder;
class Skeleton;

class cMeshRenderer : public Component
{
public:
	MYLIB_REFLECTABLE();
	cMeshRenderer() = delete; // Create a generic cube
	cMeshRenderer(const SY::UUID anOwnerId,GameObjectManager* aManager); // Create a generic cube  
	cMeshRenderer(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aFilePath,bool useExact = false);
	~cMeshRenderer() override = default;
	void Render() override;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void SetNewMesh(const std::shared_ptr<Mesh> aMesh);
	void SetMaterialPath(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath,int elementIndex);

	void SetMaterial(const std::shared_ptr<Material> aMaterial);
	void SetMaterial(const std::shared_ptr<Material> aMaterial,int elementIndex);

	std::shared_ptr<Material> GetMaterial(int materialIndex = 0) const;
	std::vector<Element>& GetElements() const;
	std::shared_ptr<Mesh> GetRawMesh() const;
	bool InspectorView() override;

	std::shared_ptr<TextureHolder> GetTexture(eTextureType type,unsigned materialIndex = 0) const;
	bool IsDefaultMesh() const;
	bool isInstanced = false;

protected:
	std::shared_ptr<Mesh> m_Mesh;
	//Meshes can have defaulted materials but the meshrenderer has the capacity to override them.
	std::vector<std::shared_ptr<Material>> m_OverrideMaterial; //TODO MAKE MAP SO INSTANCE CAN KEEP ANY OVERRIDEN MATERIAL YOU MUPPET 
};

REFL_AUTO(
	type(cMeshRenderer),
	field(isInstanced)
)

class cSkeletalMeshRenderer : public cMeshRenderer
{
	friend class cAnimator;
public:
	cSkeletalMeshRenderer() = delete;
	cSkeletalMeshRenderer(const SY::UUID anOwnerId,GameObjectManager* aManager);
	cSkeletalMeshRenderer(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aFilePath);
	~cSkeletalMeshRenderer() override = default;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void Render() override;
	bool InspectorView() override;

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

void PopUpContextForAsset();
