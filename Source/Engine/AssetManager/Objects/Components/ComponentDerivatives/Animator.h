#pragma once 
#include "../Component.h"

#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <Tools/Utilities/LinearAlgebra/Vector3.hpp>
#include "MeshRenderer.h"

#define boneLimit 128

enum class eAnimationState
{
	Stopped,
	Playing,
	Ended,
	Paused
};



class GfxCmd_RenderSkeletalMesh;
struct RenderData;

class cAnimator : public Component
{
public:
	cAnimator() = delete; // Create a generic cube
	cAnimator(const unsigned int anOwnerId); // Create a generic cube 
	cAnimator(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	~cAnimator() = default;

	void Update() override;

	void RenderAnimation(const RenderData& aMesh,const CU::Matrix4x4<float> aTransform);
	void AddAnimation(Animation aAnimation);
	void AddAnimation(const std::filesystem::path& aFilePath);
	eAnimationState GetState() const;
	void SetState(eAnimationState aState);
	void SetPlayingAnimation(unsigned int aAnimationIndex);
private:
	void SetHierarchy(unsigned int aBoneID,CU::Matrix4x4<float> aParentMatrix);
	void UpdateAnimationHierarcy(float t);
	eAnimationState myState;
	int myCurrentAnimation;
	float myAnimationTimer;
	unsigned int myCurrentFrame;
	std::array<CU::Matrix4x4<float>,boneLimit> myBoneTransforms;
	std::vector < std::shared_ptr<Animation> > myAnimations;
	std::shared_ptr<Skeleton> mySkeleton;
};

