#include "Engine/AssetManager/AssetManager.pch.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"

#include "Engine/AssetManager/ComponentSystem/Components/Animator.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/AssetManager/Objects/BaseAssets/Animations.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/ImGui/ImGui/ImGuiHepers.hpp"
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include <unordered_set>

#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h"


//Must define function EditorIcon for asset
template<typename asset = Mesh>
inline void PopUpContextForAsset(std::shared_ptr<asset>& replace)
{
	if (ImGui::BeginPopupContextItem())
	{
		ImGui::BeginTable(replace->GetTypeInfo().Name().c_str(), 2, 0, { 0,500 });
		ImGui::TableNextColumn();
		const auto& assetMap = AssetManager::Get().GetLibraryOfType<asset>()->GetContentCatalogue<asset>();
		for (const auto& [path, content] : assetMap)
		{
			if (!content)
			{
				continue;
			}

			ImGui::Text(path.stem().string().c_str());
			ImGui::TableNextColumn();
			if (ImGui::ImageButton(("PopUpContextMenu" + path.string()).c_str(), content->GetEditorIcon(), { 100,100 }))
			{
				replace = AssetManager::Get().LoadAsset<asset>(path);
				ImGui::CloseCurrentPopup();
			}
			ImGui::TableNextColumn();
		}
		ImGui::EndTable();
		ImGui::EndPopup();
	}
}

cMeshRenderer::cMeshRenderer(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
	m_Mesh = AssetManager::Get().LoadAsset<Mesh>("default.fbx");
}

inline cMeshRenderer::cMeshRenderer(const SY::UUID anOwnerId, GameObjectManager* aManager, const std::filesystem::path& aFilePath, bool useExact) : Component(anOwnerId, aManager)
{
	m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath, useExact);
}

void cMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath);
}

void cMeshRenderer::SetNewMesh(std::shared_ptr<Mesh> aMesh)
{
	m_Mesh = aMesh;
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath)
{
	SetMaterial(AssetManager::Get().LoadAsset<Material>(aFilePath), 0);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path& aFilePath, int elementIndex)
{
	SetMaterial(AssetManager::Get().LoadAsset<Material>(aFilePath), elementIndex);
}

void cMeshRenderer::SetMaterial(const std::shared_ptr<Material> aMaterial)
{
	SetMaterial(aMaterial, 0);
}

void cMeshRenderer::SetMaterial(const std::shared_ptr<Material> aMaterial, int elementIndex)
{
	if (m_OverrideMaterial.size() <= elementIndex)
	{
		m_OverrideMaterial.resize(elementIndex + 1);
	}
	m_OverrideMaterial[elementIndex] = aMaterial;
}

std::shared_ptr<Material> cMeshRenderer::GetMaterial(int materialIndex) const
{
	assert(materialIndex >= 0);

	if (!m_OverrideMaterial.empty() && m_OverrideMaterial.size() > materialIndex)
	{
		if (const auto mat = m_OverrideMaterial[materialIndex])
		{
			return mat;
		};
	}

	if (m_Mesh->materials.contains(materialIndex))
	{
		if (const auto mat = m_Mesh->materials[materialIndex])
		{
			return mat;
		};
	}

	return nullptr;
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

bool cMeshRenderer::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<cMeshRenderer>();
	{
		if (ImGui::TreeNodeEx("Static meshes", ImGuiTreeNodeFlags_DefaultOpen)) // Replace with element name
		{
			ImGui::BeginTable("Mesh", 2);
			ImGui::TableNextColumn();
			ImGui::Text("Static Mesh");
			ImGui::TableNextColumn();
			ImGui::ImageButton("WeirdUniqueIdentifer11231941241", m_Mesh->GetEditorIcon(), { 100,100 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentAsset_Mesh"))
				{
					const auto path = std::string((const char*)payload->Data, payload->DataSize);
					m_Mesh = AssetManager::Get().LoadAsset<Mesh>(path);
				}
				ImGui::EndDragDropTarget();
			}
			PopUpContextForAsset<Mesh>(m_Mesh);
			ImGui::SetItemTooltip("Replace mesh");
			ImGui::EndTable();
			ImGui::Separator();
			ImGui::TreePop();
		}
	}


	if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_DefaultOpen)) // Replace with element name
	{
		int index = 0;
		std::unordered_set<uint32_t> materialIndexes;
		for (const auto& element : m_Mesh->Elements)
		{
			materialIndexes.emplace(element.MaterialIndex);
		}

		for (const auto& matIndex : materialIndexes)
		{
			const auto textIndex = std::format("Material {}:", index);
			ImGui::BeginTable(textIndex.c_str(), 2);
			ImGui::TableNextColumn();
			ImGui::Text(textIndex.c_str());
			ImGui::TableNextColumn();

			if (m_OverrideMaterial.size() > matIndex && m_OverrideMaterial[matIndex])
			{
				if (ImGui::ImageButton("WeirdUniqueIdentifer512512313241", m_OverrideMaterial[matIndex]->GetEditorIcon(), { 100,100 }, ImGuiButtonFlags_PressedOnDoubleClick))
				{
					auto newWindow = std::make_shared<CustomFuncWindow>(std::bind(&Material::InspectorView, m_OverrideMaterial[matIndex]));

					Editor::Get().g_EditorWindows.emplace_back(newWindow);
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentAsset_Material"))
					{
						if (payload)
						{
							const auto path = std::string((const char*)payload->Data, payload->DataSize);
							m_OverrideMaterial[matIndex] = AssetManager::Get().LoadAsset<Material>(path);
						}
					}
					ImGui::EndDragDropTarget();
				}
				PopUpContextForAsset<Material>(m_OverrideMaterial[matIndex]);

			}
			else if (m_Mesh->materials.contains(matIndex))
			{
				if(ImGui::ImageButton("WeirdUniqueIdentife124675471", m_Mesh->materials.at(matIndex)->GetEditorIcon(), { 100,100 }, ImGuiButtonFlags_PressedOnDoubleClick))
				{
					auto newWindow = std::make_shared<CustomFuncWindow>(std::bind(&Material::InspectorView, m_Mesh->materials.at(matIndex)));

					Editor::Get().g_EditorWindows.emplace_back(newWindow);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentAsset_Material"))
					{
						if (payload)
						{
							const auto path = std::string((const char*)payload->Data, payload->DataSize);
							m_Mesh->materials.at(matIndex) = AssetManager::Get().LoadAsset<Material>(path);
							Logger::Log(path);
						}
					}
					ImGui::EndDragDropTarget();
				}
				PopUpContextForAsset<Material>(m_Mesh->materials.at(matIndex));
			}

			ImGui::Separator();
			index++;
			ImGui::EndTable();
		}

		ImGui::TreePop();
	}

	return true;
}


std::shared_ptr<TextureHolder> cMeshRenderer::GetTexture(eTextureType type, unsigned materialIndex) const
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
			}
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

	for (const auto& [value, material] : m_Mesh->materials)
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










cSkeletalMeshRenderer::cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager* aManager) : cMeshRenderer(anOwnerId, aManager)
{
	//cMeshRenderer creates default mesh
}

cSkeletalMeshRenderer::cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager* aManager, const std::filesystem::path& aFilePath) : cMeshRenderer(anOwnerId, aManager, aFilePath)
{
	mySkeleton = AssetManager::Get().LoadAsset<Skeleton>(aFilePath);
	assert(mySkeleton);
}

void cSkeletalMeshRenderer::SetNewMesh(const std::filesystem::path& aFilePath)
{
	m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath);
	mySkeleton = AssetManager::Get().LoadAsset<Skeleton>(aFilePath);
}

void cSkeletalMeshRenderer::Render()
{
	OPTICK_EVENT();
	if (auto* myTransform = this->TryGetComponent<Transform>())
	{
		if (const auto* myAnimation = this->TryGetComponent<cAnimator>())
		{
			myAnimation->RenderAnimation(m_Mesh, myTransform->GetTransform());
			return;
		}
		return;
	}
}

bool cSkeletalMeshRenderer::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<cSkeletalMeshRenderer>();
	return true;
}