#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>  
#include <Tools/Utilities/System/ThreadPool.hpp> 

cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myRenderData = std::make_shared<RenderData>();
	AssetManager::Get().LoadAsset<Mesh>("default.fbx",myRenderData->myMesh);
}

inline cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath,bool useExact) : Component(anOwnerId)
{
	myRenderData = std::make_shared<RenderData>();
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,useExact,myRenderData->myMesh);
	//ThreadPool::Get().SubmitWork(std::bind(&waitForLoad,myRenderData->myMesh,myOwnerID));
}

void cMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,myRenderData->myMesh);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath)
{
	SetMaterialPath(aFilePath,0);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath,int elementIndex)
{
	std::shared_ptr<Material> mat;
	AssetManager::Get().LoadAsset<Material>(aFilePath,mat);
	if (myRenderData->overrideMaterial.size() <= elementIndex)
	{
		myRenderData->overrideMaterial.resize(elementIndex + 1);
	}
	myRenderData->overrideMaterial[elementIndex] = mat;
}

void cMeshRenderer::Render()
{
	OPTICK_EVENT();
	if (auto* myTransform = this->TryGetComponent<Transform>())
	{
		if (!isInstanced)
		{
			GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
			GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform(),isInstanced);
			return;
		}
		myRenderData->myMesh->myInstances.emplace_back(myTransform->GetTransform());
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
		return;
	}
	if (!isInstanced)
	{
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),isInstanced);
		GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,Matrix(),isInstanced);
	}
	myRenderData->myMesh->myInstances.emplace_back(Matrix());
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),isInstanced);
}

cSkeletalMeshRenderer::cSkeletalMeshRenderer(const unsigned int anOwnerId) : cMeshRenderer(anOwnerId)
{
	//cMeshRenderer creates default mesh
}

cSkeletalMeshRenderer::cSkeletalMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : cMeshRenderer(anOwnerId,aFilePath)
{
	AssetManager::Get().LoadAsset<Skeleton>(aFilePath,mySkeleton);
}

void cSkeletalMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,myRenderData->myMesh);
	AssetManager::Get().LoadAsset<Skeleton>(aFilePath,mySkeleton);
}

void cSkeletalMeshRenderer::Render()
{
	OPTICK_EVENT();
	if (auto* myTransform = this->TryGetComponent<Transform>())
	{
		if (const auto* myAnimation = this->TryGetComponent<cAnimator>())
		{
			myAnimation->RenderAnimation(myRenderData,myTransform->GetTransform());
			return;
		}
		GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform(),false);
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),false);
		return;
	}
	GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,Matrix(),false);
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),false);
}
