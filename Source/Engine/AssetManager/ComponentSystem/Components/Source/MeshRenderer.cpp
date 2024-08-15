#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/AssetManager.pch.h"

#include "Engine/AssetManager/ComponentSystem/Components/Animator.h"
#include "Engine/AssetManager/Objects/BaseAssets/Animations.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/ImGui/ImGui/ImGuiHelpers.hpp"
#include "Tools/ImGui/ImGui/imgui.h"
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include <unordered_set>

#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h"

// Must define function EditorIcon for asset

cMeshRenderer::cMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager) : Component(anOwnerId, aManager)
{
    m_Mesh = AssetManager::Get().LoadAsset<Mesh>("default.fbx");
}

inline cMeshRenderer::cMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager,
                                    const std::filesystem::path &aFilePath, bool useExact)
    : Component(anOwnerId, aManager)
{
    m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath, useExact);
}

void cMeshRenderer::SetNewMesh(const std::filesystem::path &aFilePath)
{
    m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath);
}

void cMeshRenderer::SetNewMesh(std::shared_ptr<Mesh> aMesh)
{
    m_Mesh = aMesh;
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path &aFilePath)
{
    SetMaterial(AssetManager::Get().LoadAsset<Material>(aFilePath), 0);
}

void cMeshRenderer::SetMaterialPath(const std::filesystem::path &aFilePath, int elementIndex)
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

std::vector<Element> &cMeshRenderer::GetElements() const
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

float cMeshRenderer::GetBoundingSphereRadius() const
{
    if (!m_Mesh)
    {
        return 0;
    }

    // TODO Returned the scaled and transformed bound
    return m_Mesh->Bounds.GetRadius();
}

AABB3D<> cMeshRenderer::GetBoundingBox() const
{
    if (!m_Mesh)
    {
        return AABB3D();
    }
    // TODO Returned the scaled and transformed bound
    return m_Mesh->Bounds;
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
            SwitchableAsset<Mesh>(m_Mesh, "ContentAsset_Mesh");
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
        for (const auto &element : m_Mesh->Elements)
        {
            materialIndexes.emplace(element.MaterialIndex);
        }

        for (const auto &matIndex : materialIndexes)
        {
            const auto textIndex = std::format("Material {}:", index);
            ImGui::BeginTable(textIndex.c_str(), 2);
            ImGui::TableNextColumn();
            ImGui::Text(textIndex.c_str());
            ImGui::TableNextColumn();

            if (m_OverrideMaterial.size() > matIndex && m_OverrideMaterial[matIndex])
            {
                SwitchableAsset<Material>(m_OverrideMaterial[matIndex], "ContentAsset_Material");
            }
            else if (m_Mesh->materials.contains(matIndex))
            {
                SwitchableAsset<Material>(m_Mesh->materials.at(matIndex), "ContentAsset_Material");
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

        for (const auto &material : m_OverrideMaterial)
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

    for (const auto &[value, material] : m_Mesh->materials)
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

cSkeletalMeshRenderer::cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager)
    : cMeshRenderer(anOwnerId, aManager)
{
    // cMeshRenderer creates default mesh
}

cSkeletalMeshRenderer::cSkeletalMeshRenderer(const SY::UUID anOwnerId, GameObjectManager *aManager,
                                             const std::filesystem::path &aFilePath)
    : cMeshRenderer(anOwnerId, aManager, aFilePath)
{
    mySkeleton = AssetManager::Get().LoadAsset<Skeleton>(aFilePath);
    assert(mySkeleton);
}

void cSkeletalMeshRenderer::SetNewMesh(const std::filesystem::path &aFilePath)
{
    m_Mesh = AssetManager::Get().LoadAsset<Mesh>(aFilePath);
    mySkeleton = AssetManager::Get().LoadAsset<Skeleton>(aFilePath);
}

void cSkeletalMeshRenderer::Render()
{
    OPTICK_EVENT();
    if (auto *myTransform = this->TryGetComponent<Transform>())
    {
        if (const auto *myAnimation = this->TryGetComponent<cAnimator>())
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
