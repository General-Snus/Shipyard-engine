#include "Engine/AssetManager/AssetManager.pch.h"
#include "Engine/AssetManager/ComponentSystem/Components/Animator.h"

#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/Objects/BaseAssets/Animations.h"

cAnimator::cAnimator(const SY::UUID anOwnerId,GameObjectManager* aManager)  
	: Component(anOwnerId,aManager),myCurrentAnimation(0),myAnimationTimer(0)
{
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if (!mySkeleton)
	{
		Logger.Err("cSkeletalMeshRenderer component does not have a skeleton"); 
	}
}

cAnimator::cAnimator(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aFilePath) : Component(anOwnerId,aManager),myCurrentAnimation(0),myAnimationTimer(0),myCurrentFrame(0)
{
	aFilePath;
	mySkeleton = (this->TryGetComponent<cSkeletalMeshRenderer>())->GetRawSkeleton();
	if (!mySkeleton)
	{
		Logger.Err("cSkeletalMeshRenderer component does not have a skeleton");
	}  
	myAnimations.push_back(EngineResources.LoadAsset<Animation>(aFilePath));
}

void cAnimator::Update()
{
	OPTICK_EVENT();

	if (myAnimations.size())
	{
		const float TimePerFrame = (1 / myAnimations[myCurrentAnimation]->frameRate);
		myAnimationTimer += TimerInstance.GetDeltaTime();
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

void cAnimator::RenderAnimation(const std::shared_ptr<Mesh>& aData,const Matrix& aTransform) const
{
	aData; aTransform;
	//GraphicsEngineInstance.ShadowCommands<GfxCmd_RenderSkeletalMeshShadow>(aData,aTransform,myBoneTransforms.data(),static_cast<unsigned int>(mySkeleton->myBones.size()));
	//GraphicsEngineInstance.DeferredCommand<GfxCmd_RenderSkeletalMesh>(aData,aTransform,myBoneTransforms.data(),static_cast<unsigned int>(mySkeleton->myBones.size()));
}

void cAnimator::AddAnimation(std::shared_ptr<Animation> aAnimation)
{
	myAnimations.push_back(aAnimation);
}

void cAnimator::AddAnimation(const std::filesystem::path& aFilePath)
{
	myAnimations.push_back(EngineResources.LoadAsset<Animation>(aFilePath));
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
		Logger.Warn("Animation index out of range");
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

bool cAnimator::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<cAnimator>();
	return true;
}