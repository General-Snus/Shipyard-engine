#include "Engine/AssetManager/AssetManager.pch.h"

#include "../GraphComponent.h"
#include <Tools/ImGui/MuninGraph/MuninGraph.h>
#include <Tools/ImGui/MuninGraph/ScriptGraph/ScriptGraph.h>

GraphComponent::GraphComponent(const SY::UUID anOwnerID, GameObjectManager *aManager) : Component(anOwnerID, aManager)
{
}

GraphComponent::GraphComponent(const SY::UUID anOwnerID, GameObjectManager *aManager, const std::filesystem::path &path)
    : Component(anOwnerID, aManager)
{
    std::shared_ptr<GraphAsset> ptr;
    AssetManager::Get().ForceLoadAsset<GraphAsset>(path, ptr);

    if (!ptr)
    {
        Logger::Warn("Could not load GraphAsset at " + path.string());
    }

    m_Graph = ptr;

    m_Graph->graphToEdit->BindErrorHandler(
        [anOwnerID, aManager](const ScriptGraph *aGraph, size_t aNodeUID, std::string_view aMessage) {
            if (aManager; auto cmp = aManager->TryGetComponent<GraphComponent>(anOwnerID))
            {
                cmp->SetActive(false);
            }
            aGraph;
            Logger::Err(std::format("Graph failure at node {} with {}", aNodeUID, aMessage));
            return;
        });
}

void GraphComponent::Update()
{
    try
    {
        if (&m_Graph && m_Graph->isLoadedComplete && m_Graph->graphToEdit)
        {
            m_Graph->graphToEdit->Tick(Timer::GetDeltaTime());
        }
    }
    catch (const std::exception &e)
    {
        Logger::Err(e.what());
    }
}

void GraphComponent::OnColliderEnter(const SY::UUID aGameObjectID)
{
    try
    {
        if (&m_Graph && m_Graph->isLoadedComplete && m_Graph->graphToEdit)
        {
            m_Payload.SetPinValue("UserID", myOwnerID);
            m_Payload.SetPinValue("CollidedWith", aGameObjectID);
            m_Graph->graphToEdit->ExecuteWithPayload("OnEnter", m_Payload);
        }
    }
    catch (const std::exception &e)
    {
        Logger::Err(e.what());
    }
}

void GraphComponent::OnColliderExit(const SY::UUID aGameObjectID)
{
    try
    {
        if (&m_Graph && m_Graph->isLoadedComplete && m_Graph->graphToEdit)
        {
            m_Payload.SetPinValue("UserID", myOwnerID);
            m_Payload.SetPinValue("CollidedWith", aGameObjectID);
            m_Graph->graphToEdit->ExecuteWithPayload("OnExit", m_Payload);
        }
    }
    catch (const std::exception &e)
    {
        Logger::Err(e.what());
    }
}

bool GraphComponent::InspectorView()
{
    if (!Component::InspectorView())
    {
        return false;
    }
    Reflect<GraphComponent>();
    if (ImGui::TreeNodeEx("Graph",
                          ImGuiTreeNodeFlags_DefaultOpen)) // Replace with element name
    {
        ImGui::BeginTable("Graph", 2);
        ImGui::TableNextColumn();
        ImGui::Text(m_Graph->AssetPath.stem().string().c_str());
        ImGui::TableNextColumn();
        SwitchableAsset<GraphAsset>(m_Graph, "ContentAsset_Graph");
        ImGui::SetItemTooltip("Replace graph");
        ImGui::EndTable();
        ImGui::Separator();
        ImGui::TreePop();
    }
    return false;
}
