#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/Enums.h"
#define AsUINT(v) static_cast<unsigned>(v)

class Mesh;
class Material;
class Skeleton;

struct RenderData
{
	//Equal operator
	inline bool operator==(const RenderData& aOther) const
	{
		return myMesh == aOther.myMesh;
	}
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> overrideMaterial; //if this exist it will override the material on the mesh
};

class cMeshRenderer : public Component
{
public:
	cMeshRenderer() = delete; // Create a generic cube
	cMeshRenderer(const unsigned int anOwnerId); // Create a generic cube  
	cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath,bool useExact = false);
	void Render() override;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath,int elementIndex);
	~cMeshRenderer() override = default;

	const std::vector<Element>& GetElements() const;
	std::shared_ptr<Mesh> GetRawMesh() const;

	std::shared_ptr<TextureHolder> GetTexture(eTextureType type);
	bool IsDefaultMesh() const;

	FORCEINLINE bool IsStaticMesh()
	{
		return isStatic;
	}

	FORCEINLINE void SetIsStaticMesh(bool aBool)
	{
		isStatic = aBool;
	}

protected:
	bool isInstanced = false;
	bool isStatic = false;
	std::shared_ptr<RenderData> myRenderData;
};


class cSkeletalMeshRenderer : public cMeshRenderer
{
public:
	cSkeletalMeshRenderer() = delete;
	cSkeletalMeshRenderer(const unsigned int anOwnerId);
	cSkeletalMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	void SetNewMesh(const std::filesystem::path& aFilePath);
	void Render() override;

	~cSkeletalMeshRenderer() = default;
private:

	FORCEINLINE const std::shared_ptr<Skeleton> GetRawSkeleton() const
	{
		return (mySkeleton);
	}
	friend class cAnimator;
	std::shared_ptr<Skeleton> mySkeleton;
};