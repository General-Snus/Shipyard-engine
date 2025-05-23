#pragma once 
#include "../Component.h"  
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"
#include "MeshRenderer.h"

#define boneLimit 128

class Animation;

enum class eAnimationState
{
	Stopped,
	Playing,
	Ended,
	Paused
};



class cAnimator : public Component
{
public:
	ReflectableTypeRegistration();
	cAnimator() = delete; 
	cAnimator(const SY::UUID anOwnerId,GameObjectManager* aManager); 
	cAnimator(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aFilePath);
	~cAnimator() override = default;

	void Update() override;

	void RenderAnimation(const std::shared_ptr<Mesh>& aData,const Matrix& aTransform) const;
	void AddAnimation(std::shared_ptr<Animation> aAnimation);
	void AddAnimation(const std::filesystem::path& aFilePath);
	eAnimationState GetState() const;
	void SetState(eAnimationState aState);
	void SetPlayingAnimation(unsigned int aAnimationIndex);
	bool InspectorView() override;
private:
	void SetHierarchy(unsigned int aBoneID,const  Matrix& aParentMatrix);
	void UpdateAnimationHierarcy(float t);
	eAnimationState myState;
	int myCurrentAnimation;
	float myAnimationTimer;
	unsigned int myCurrentFrame;
	std::array<Matrix,boneLimit> myBoneTransforms;
	std::vector < std::shared_ptr<Animation> > myAnimations;
	std::shared_ptr<Skeleton> mySkeleton;
};

REFL_AUTO(type(cAnimator))
