#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>  
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myRenderData = std::make_shared<RenderData>();
	myRenderData->myMaterials.resize(1);
	AssetManager::GetInstance().LoadAsset<Mesh>("default.fbx",myRenderData->myMesh);

	myRenderData->myMaterials[0] = GraphicsEngine::Get().GetDefaultMaterial();
}

inline cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId)
{
	myRenderData = std::make_shared<RenderData>();
	myRenderData->myMaterials.resize(1);
	AssetManager::GetInstance().LoadAsset<Mesh>(aFilePath,myRenderData->myMesh);
	myRenderData->myMaterials[0] = GraphicsEngine::Get().GetDefaultMaterial();
}

void cMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	AssetManager::GetInstance().LoadAsset<Mesh>(aFilePath,myRenderData->myMesh);
}
void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath)
{
	std::shared_ptr<Material> mat;
	AssetManager::GetInstance().LoadAsset<Material>(aFilePath,mat);
	myRenderData->myMaterials[0] = mat; // REFACTOR : 0 is the wanted element,   
}
void cMeshRenderer::Render()
{
	OPTICK_EVENT();
	if(const auto* myTransform = this->TryGetComponent<Transform>())
	{
		if(!isInstanced)
		{
			GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
			GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform(),isInstanced);
			return;
		} 
		myRenderData->myMesh->myInstances.emplace_back(myTransform->GetTransform());
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
		return;
	}
	if(!isInstanced)
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
	AssetManager::GetInstance().LoadAsset<Skeleton>(aFilePath,mySkeleton);
}

void cSkeletalMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	AssetManager::GetInstance().LoadAsset<Mesh>(aFilePath,myRenderData->myMesh);
	AssetManager::GetInstance().LoadAsset<Skeleton>(aFilePath,mySkeleton);
}

void cSkeletalMeshRenderer::Render()
{
	OPTICK_EVENT();
	if(const auto* myTransform = this->TryGetComponent<Transform>())
	{ 
		if(const auto* myAnimation = this->TryGetComponent<cAnimator>())
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
