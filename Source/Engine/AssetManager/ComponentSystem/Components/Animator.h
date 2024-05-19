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



struct RenderData;

class cAnimator : public Component
{
public:
	MYLIB_REFLECTABLE();
	cAnimator() = delete; // Create a generic cube
	cAnimator(const unsigned int anOwnerId); // Create a generic cube 
	cAnimator(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);
	~cAnimator() override = default;

	void Update() override;

	void RenderAnimation(const std::shared_ptr<RenderData>& aData,const Matrix& aTransform) const;
	void AddAnimation(Animation aAnimation);
	void AddAnimation(const std::filesystem::path& aFilePath);
	eAnimationState GetState() const;
	void SetState(eAnimationState aState);
	void SetPlayingAnimation(unsigned int aAnimationIndex);
	void InspectorView() override;
private:
	void SetHierarchy(unsigned int aBoneID,const  Matrix& aParentMatrix);
	void UpdateAnimationHierarcy(float t);
	eAnimationState myState;
	int myCurrentAnimation;
	float myAnimationTimer;
	unsigned int myCurrentFrame;
	std::array< Matrix,boneLimit> myBoneTransforms;
	std::vector < std::shared_ptr<Animation> > myAnimations;
	std::shared_ptr<Skeleton> mySkeleton;
};

REFL_AUTO(type(cAnimator))
