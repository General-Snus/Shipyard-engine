#include "AssetManager.pch.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/ImGui/ImGui/imgui.h"

class GfxCmd_RenderMesh;

cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId) : Component(anOwnerId)
{
	AssetManager::Get().LoadAsset<Mesh>("default.fbx",m_Mesh);
}

inline cMeshRenderer::cMeshRenderer(const unsigned int anOwnerId,const std::filesystem::path& aFilePath,bool useExact) : Component(anOwnerId)
{
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,useExact,m_Mesh);
}

void cMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,m_Mesh);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath)
{
	SetMaterialPath(aFilePath,0);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath,int elementIndex)
{
	std::shared_ptr<Material> mat;
	AssetManager::Get().LoadAsset<Material>(aFilePath,mat);
	if (m_OverrideMaterial.size() <= elementIndex)
	{
		m_OverrideMaterial.resize(elementIndex + 1);
	}
	m_OverrideMaterial[elementIndex] = mat;
}

bool cMeshRenderer::IsDefaultMesh() const
{
	if (m_Mesh->AssetPath == L"../../Content/default.fbx") // What am i doing with my life
	{
		return true;
	}
	return false;
}

std::vector<Element>& cMeshRenderer::GetElements() const
{
	OPTICK_EVENT();
	if (m_Mesh->isLoadedComplete)
	{
		return m_Mesh->Elements;
	}
	else
	{
		static std::vector<Element> empty;
		return empty;
	}
}

std::shared_ptr<Mesh> cMeshRenderer::GetRawMesh() const
{
	return m_Mesh;
}

void cMeshRenderer::InspectorView()
{
	Component::InspectorView();
	Reflect<cMeshRenderer>();
}

std::shared_ptr<TextureHolder> cMeshRenderer::GetTexture(eTextureType type,unsigned materialIndex)
{
	OPTICK_EVENT();
	assert(materialIndex >= 0);

	if (!m_OverrideMaterial.empty())
	{
		if (m_OverrideMaterial.size() > materialIndex)
		{
			if (const auto mat = m_OverrideMaterial[materialIndex])
			{
				if (const auto tex = mat->GetTexture(type))
				{
					return tex;
				}
			};
		}

		for (const auto& material : m_OverrideMaterial)
		{
			if (!material)
			{
				continue;
			}

			if (auto tex = material->GetTexture(type))
			{
				return tex;
			}
		}
	}

	if (m_Mesh->materials.size() > materialIndex)
	{
		if (const auto mat = m_Mesh->materials[materialIndex])
		{
			if (const auto tex = mat->GetTexture(type))
			{
				return tex;
			}
		};
	}

	for (const auto& [value,material] : m_Mesh->materials)
	{
		if (!material)
		{
			continue;
		}

		if (const auto tex = material->GetTexture(type))
		{
			return tex;
		}
	}

	return nullptr;
}
void cMeshRenderer::Render()
{
	OPTICK_EVENT();
	//if (auto* myTransform = this->TryGetComponent<Transform>())
	//{
	//	if (!isInstanced)
	//	{
	//		GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
	//		//GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform(),isInstanced);
	//		return;
	//	}
	//	myRenderData->myMesh->myInstances.emplace_back(myTransform->GetTransform());
	//	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),isInstanced);
	//	return;
	//}
	//if (!isInstanced)
	//{
	//	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),isInstanced);
	//	//GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,Matrix(),isInstanced);
	//}
	//myRenderData->myMesh->myInstances.emplace_back(Matrix());
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),isInstanced);
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
	AssetManager::Get().LoadAsset<Mesh>(aFilePath,m_Mesh);
	AssetManager::Get().LoadAsset<Skeleton>(aFilePath,mySkeleton);
}

void cSkeletalMeshRenderer::Render()
{
	OPTICK_EVENT();
	if (auto* myTransform = this->TryGetComponent<Transform>())
	{
		if (const auto* myAnimation = this->TryGetComponent<cAnimator>())
		{
			myAnimation->RenderAnimation(m_Mesh,myTransform->GetTransform());
			return;
		}
		//	GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,myTransform->GetTransform(),false);
		//	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,myTransform->GetTransform(),false);
		return;
	}
	//GraphicsEngine::Get().ShadowCommands<GfxCmd_RenderMeshShadow>(myRenderData,Matrix(),false);
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(myRenderData,Matrix(),false);
}

void cSkeletalMeshRenderer::InspectorView()
{
	Component::InspectorView();
	Reflect<cSkeletalMeshRenderer>();
}