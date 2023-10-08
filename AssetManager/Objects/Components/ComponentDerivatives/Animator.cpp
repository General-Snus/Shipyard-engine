#include "AssetManager.pch.h"
#include "Animator.h"
#include "Transform.h" 
#include <GraphicsEngine/Commands/GraphicCommands.h>

cAnimator::cAnimator(const unsigned int anOwnerId) : Component(anOwnerId),myCurrentAnimation(0),myAnimationTimer(0)
{
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if(!mySkeleton)
	{
		std::cout << "cSkeletalMeshRenderer component does not have a skeleton" << std::endl; 
	}
}

cAnimator::cAnimator(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId),myCurrentAnimation(0),myAnimationTimer(0),myCurrentFrame(0)
{
	aFilePath;
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if(!mySkeleton)
	{
		std::cout << "cSkeletalMeshRenderer component does not have a skeleton" << std::endl;
	}
	std::shared_ptr<Animation> animation;
	AssetManager::GetInstance().LoadAsset<Animation>(aFilePath,animation);
	myAnimations.push_back(animation);
}

void cAnimator::Update()
{
	
	if(myAnimations.size())
	{
		const float TimePerFrame = (1 / myAnimations[myCurrentAnimation]->frameRate);
		myAnimationTimer += CU::Timer::GetInstance().GetDeltaTime();
		if(myAnimationTimer >= TimePerFrame)
		{
			float percentage = myAnimationTimer / TimePerFrame;
			myAnimationTimer = 0;
			myCurrentFrame++;

			if(!myAnimations[myCurrentAnimation]->isLoadedComplete)
			{
				return;
			}

			if(myCurrentFrame >= myAnimations[myCurrentAnimation]->numFrames)
			{
				myCurrentFrame = 0;
			}
			UpdateAnimationHierarcy(percentage);
		}
	}
}

void cAnimator::RenderAnimation(const RenderData& aData,const CU::Matrix4x4<float> aTransform)
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
	AssetManager::GetInstance().LoadAsset<Animation>(aFilePath,animation);
	myAnimations.push_back(animation);
}

void cAnimator::SetHierarchy(unsigned int aBoneID,CU::Matrix4x4<float> aParentMatrix)
{
	std::string boneName = mySkeleton->myBones[aBoneID].Name;
	CU::Matrix4x4<float> newBoneTransform = myAnimations[myCurrentAnimation]->Frames[myCurrentFrame].myTransforms[boneName] * aParentMatrix;

	for(unsigned int i : mySkeleton->myBones[aBoneID].Children)
	{
		SetHierarchy(i,newBoneTransform);
	}
	myBoneTransforms[aBoneID] = mySkeleton->myBones[aBoneID].BindPoseInverse * newBoneTransform ; 
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
	if(aAnimationIndex >= myAnimations.size())
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
	Transform* transform = this->TryGetComponent<Transform>();
	if(transform != nullptr)
	{
		SetHierarchy(0,CU::Matrix4x4<float>());
		return;
	}
	SetHierarchy(0,CU::Matrix4x4<float>());

}