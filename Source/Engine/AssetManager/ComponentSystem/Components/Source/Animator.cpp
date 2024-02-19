#include "AssetManager.pch.h" 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include "../Animator.h" 

#include "Engine/GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderSkeletalMeshShadow.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"

cAnimator::cAnimator(const unsigned int anOwnerId) : Component(anOwnerId),myCurrentAnimation(0),myAnimationTimer(0)
{
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if (!mySkeleton)
	{
		std::cout << "cSkeletalMeshRenderer component does not have a skeleton" << std::endl;
	}
}

cAnimator::cAnimator(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId),myCurrentAnimation(0),myAnimationTimer(0),myCurrentFrame(0)
{
	aFilePath;
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if (!mySkeleton)
	{
		std::cout << "cSkeletalMeshRenderer component does not have a skeleton" << std::endl;
	}
	std::shared_ptr<Animation> animation;
	AssetManager::Get().LoadAsset<Animation>(aFilePath,animation);
	myAnimations.push_back(animation);
}

void cAnimator::Update()
{
	OPTICK_EVENT();

	if (myAnimations.size())
	{
		const float TimePerFrame = (1 / myAnimations[myCurrentAnimation]->frameRate);
		myAnimationTimer += Timer::GetInstance().GetDeltaTime();
		if (myAnimationTimer >= TimePerFrame)
		{
			float percentage = myAnimationTimer / TimePerFrame;
			myAnimationTimer = 0;
			myCurrentFrame++;

			if (!myAnimations[myCurrentAnimation]->isLoadedComplete)
			{
				return;
			}

			if (myCurrentFrame >= myAnimations[myCurrentAnimation]->numFrames)
			{
				myCurrentFrame = 0;
			}
			UpdateAnimationHierarcy(percentage);
		}
	}
}

void cAnimator::RenderAnimation(const std::shared_ptr<RenderData>& aData,const Matrix& aTransform) const
{
	GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderSkeletalMeshShadow>(aData,aTransform,myBoneTransforms.data(),static_cast<unsigned int>(mySkeleton->myBones.size()));
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderSkeletalMesh>(aData,aTransform,myBoneTransforms.data(),static_cast<unsigned int>(mySkeleton->myBones.size()));
}

void cAnimator::AddAnimation(Animation aAnimation)
{
	myAnimations.push_back(std::make_shared<Animation>(aAnimation));
}

void cAnimator::AddAnimation(const std::filesystem::path& aFilePath)
{
	std::shared_ptr<Animation> animation;
	AssetManager::Get().LoadAsset<Animation>(aFilePath,animation);
	myAnimations.push_back(animation);
}

void cAnimator::SetHierarchy(unsigned int aBoneID,const Matrix& aParentMatrix)
{
	std::string boneName = mySkeleton->myBones[aBoneID].Name;

	Matrix newBoneTransform = myAnimations[myCurrentAnimation]->Frames[myCurrentFrame].myTransforms[boneName] * aParentMatrix;

	for (unsigned int i : mySkeleton->myBones[aBoneID].Children)
	{
		SetHierarchy(i,newBoneTransform);
	}
	myBoneTransforms[aBoneID] = mySkeleton->myBones[aBoneID].BindPoseInverse * newBoneTransform;
}

eAnimationState cAnimator::GetState() const
{
	return myState;
}

void cAnimator::SetState(eAnimationState aState)
{
	myState = aState;
}

void cAnimator::SetPlayingAnimation(unsigned int aAnimationIndex)
{
	if (aAnimationIndex >= myAnimations.size())
	{
		std::cout << "Animation index out of range" << std::endl;
		return;
	}
	myCurrentAnimation = aAnimationIndex;
	myCurrentFrame = 0;
	myAnimationTimer = 0;
}

void cAnimator::UpdateAnimationHierarcy(float t)
{
	t;
	if (mySkeleton->isLoadedComplete)
	{
		auto* transform = TryGetComponent<Transform>();
		if (transform)
		{
			SetHierarchy(0,Matrix());
			return;
		}
		SetHierarchy(0,Matrix());
	}

}