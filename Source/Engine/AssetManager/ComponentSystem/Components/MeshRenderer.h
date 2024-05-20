#pragma once
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/Enums.h"
#include "Engine/AssetManager/Objects/BaseAssets/MeshAsset.h"
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
REFL_AUTO(
	type(RenderData),
	field(myMesh),
	field(overrideMaterial)
)


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

	std::shared_ptr<TextureHolder> GetTexture(eTextureType type,unsigned materialIndex = 0);
	bool IsDefaultMesh() const;

	FORCEINLINE bool IsStaticMesh()
	{
		return isStatic;
	}

	FORCEINLINE void SetIsStaticMesh(bool aBool)
	{
		isStatic = aBool;
	}

	Mesh  m_Mesh;
	bool isInstanced = false;
	bool isStatic = false;
	std::shared_ptr<RenderData> myRenderData;
};

REFL_AUTO(
	type(cMeshRenderer),
	func(GetRawMesh),
	field(m_Mesh),
	field(isInstanced),
	field(isStatic),
	field(myRenderData)
)














class cSkeletalMeshRenderer : public cMeshRenderer
{
public:
	cSkeletalMeshRenderer() = delete;
	cSkeletalMeshRenderer(const unsigned int anOwnerId);
	cSkeletalMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	void SetNewMesh(const std::filesystem::path& aFilePath);
	void Render() override;
	void InspectorView() override;

	~cSkeletalMeshRenderer() override = default;
private:

	FORCEINLINE const std::shared_ptr<Skeleton> GetRawSkeleton() const
	{
		return (mySkeleton);
	}
	friend class cAnimator;
	std::shared_ptr<Skeleton> mySkeleton;
};

REFL_AUTO(type(cSkeletalMeshRenderer))