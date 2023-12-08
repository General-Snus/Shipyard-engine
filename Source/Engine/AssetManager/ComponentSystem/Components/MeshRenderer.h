#pragma once
#include <Engine/AssetManager/AssetManager.pch.h> 

#define AsUINT(v) static_cast<unsigned>(v)
 
struct RenderData
{ 
	//Equal operator
	inline bool operator==(const RenderData& aOther) const
	{ 
		return myMesh == aOther.myMesh;
	}
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterials;
};
 


class cMeshRenderer : public Component
{
public:
	cMeshRenderer() = delete; // Create a generic cube
	cMeshRenderer(const unsigned int anOwnerId); // Create a generic cube  
	cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	bool TrySetMaterialFromMesh();
	void Render() override;

	void SetNewMesh(const std::filesystem::path& aFilePath);
	void SetMaterialPath(const std::filesystem::path& aFilePath);
	~cMeshRenderer() override = default;

	FORCEINLINE const std::vector<Element>& GetElements() const
	{
		return myRenderData->myMesh->Elements;
	}
	FORCEINLINE std::shared_ptr<Mesh> GetRawMesh() const
	{
		return myRenderData->myMesh;
	}

	FORCEINLINE bool IsDefaultMesh()
	{
		if(myRenderData->myMesh->AssetPath == L"../../Content/default.fbx") // What am i doing with my life
		{
			return true;
		}
		return false;
	}

protected:
	bool isInstanced = true;
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