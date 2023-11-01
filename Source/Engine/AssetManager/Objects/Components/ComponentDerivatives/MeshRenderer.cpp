#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include "MeshRenderer.h"
#include "Transform.h"
#include "Animator.h" 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myRenderData = new RenderData();
	myRenderData->myMaterials.resize(1);
	AssetManager::GetInstance().LoadAsset<Mesh>("default.fbx",myRenderData->myMesh);

	myRenderData->myMaterials[0] = GraphicsEngine::Get().GetDefaultMaterial();
}

inline cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId)
{
	myRenderData = new RenderData();
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
	Transform* myTransform = this->TryGetComponent<Transform>();
	if(myTransform != nullptr)
	{
		myRenderData->myMesh->myInstances.push_back(myTransform->GetTransform());
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform());
		GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform());
		return;
	}
	myRenderData->myMesh->myInstances.push_back(Matrix());
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,CU::Matrix4x4<float>());
	GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,CU::Matrix4x4<float>());
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
	Transform* myTransform = this->TryGetComponent<Transform>();
	if(myTransform != nullptr)
	{
		cAnimator* myAnimation = this->TryGetComponent<cAnimator>();
		myRenderData->myMesh->myInstances.push_back(myTransform->GetTransform());
		if(myAnimation != nullptr)
		{
			myAnimation->RenderAnimation(myRenderData,myTransform->GetTransform());
			return;
		}
		GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform());
		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform());
		return;
	}
	myRenderData->myMesh->myInstances.push_back(Matrix());
	GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,CU::Matrix4x4<float>());
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,CU::Matrix4x4<float>());
}
