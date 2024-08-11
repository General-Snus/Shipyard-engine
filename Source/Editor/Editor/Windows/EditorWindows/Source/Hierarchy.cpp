#include "../Hierarchy.h"

#include "Editor/Editor/Commands/SceneAction.h"
#include "Editor/Editor/Core/Editor.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Engine/PersistentSystems/Scene.h"
#include "ImGuiHepers.hpp"
#include "Tools/Utilities/Input/Input.hpp"
#include "Tools/Utilities/Math.hpp"
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Font/IconsFontAwesome5.h>
#include <format>

#undef min
#undef max

void Hierarchy::PopupMenu(SY::UUID id)
{
    const auto popupId = "##Hierarchy" + static_cast<std::string>(id);
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        if (ImGui::Selectable("Create Empty"))
        {
            const auto ptr = std::make_shared<GameobjectAdded>(GameObject::Create("Empty"));
            CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
        }
        if (ImGui::Selectable("Create Empty Child") && id.IsValid())
        {
            auto parent = Scene::ActiveManager().GetGameObject(id);
            auto child = GameObject::Create("Empty child");
            child.transform().SetParent(parent.transform());

            const auto ptr = std::make_shared<GameobjectAdded>(child);
            CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
        }
        ImGui::Separator();
        if (ImGui::Selectable("Duplicate") && id.IsValid())
        {
            auto gameObject = Scene::ActiveManager().GetGameObject(id);
            auto components = gameObject.CopyAllComponents();

            auto newObject = GameObject::Create(gameObject.GetName() + "_1");
            for (const auto &i : components)
            {
                newObject.AddBaseComponent(i);
            }

            const auto ptr = std::make_shared<GameobjectAdded>(newObject);
            CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
        }
        if (ImGui::Selectable("Delete") && id.IsValid())
        {
            // TODO add do in add command function its more logical that way

            CommandPacket packet;
            GameObject obj = Scene::ActiveManager().GetGameObject(id);
            const auto parentCommand = std::make_shared<GameobjectDeleted>(obj);

            // for (auto const &i : obj.transform().GetAllChildren())
            //{
            //     const auto ptr = std::make_shared<GameobjectDeleted>(i.get().GetGameObject());
            //     Scene::ActiveManager().DeleteGameObject(i.get().GetOwner(), true);
            //     packet.emplace_back(ptr);
            // }
            packet.emplace_back(parentCommand);
            Scene::ActiveManager().DeleteGameObject(id, true);

            CommandBuffer::MainEditorCommandBuffer().AddCommand(packet);
            CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
        }
        ImGui::Separator();
        if (ImGui::Selectable("Rename") && id.IsValid())
        {
            auto gameObject = Scene::ActiveManager().GetGameObject(id);
            gameObject.SetName("Renamed GameObject");
        }
        ImGui::Separator();
        if (ImGui::Selectable("Copy"))
        {
            Editor::Copy();
        }
        if (ImGui::Selectable("Paste"))
        {
            Editor::Paste();
        }
        ImGui::Separator();
        if (ImGui::Selectable("Move camera to object") && id.IsValid())
        {
            Editor::Get().FocusObject(Scene::ActiveManager().GetGameObject(id));
        }
        if (ImGui::Selectable("Align With View") && id.IsValid())
        {
            // Move object to align with scene camera
            Editor::Get().AlignObject(Scene::ActiveManager().GetGameObject(id));
        }
        if (ImGui::Selectable("Align View to Selected") && id.IsValid())
        {
            // Move scene camera to align with selected object
            Editor::Get().FocusObject(Scene::ActiveManager().GetGameObject(id), false);
        }
        if (ImGui::Selectable("Set as Parent") && id.IsValid())
        {
            auto selected = Editor::GetSelectedGameObjects();

            for (auto &child : selected)
            {
                // TODO Whole component change function
                auto &parentTransform = Scene::ActiveManager().GetGameObject(id).transform();
                child.transform().SetParent(parentTransform);
            }
        }
        if (ImGui::Selectable("Clear Parent") && id.IsValid())
        {
            auto gameObject = Scene::ActiveManager().GetGameObject(id);
            gameObject.transform().Detach();
        }
        ImGui::Separator();
        if (ImGui::Selectable("Add Component") && id.IsValid())
        {
        }
        ImGui::EndPopup();
    }
}

void Hierarchy::RenderNode(Transform &transform)
{
    auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen |
                 ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

    const auto &style = ImGui::GetStyle();
    auto color = style.Colors[ImGuiCol_Text];

    const auto &data = transform.GetGameObject();
    auto id = data.GetID();
    bool isSelected = false;
    const auto &selectedObjects = Editor::GetSelectedGameObjects();
    for (const auto &i : selectedObjects)
    {
        if (i.GetID() == id)
        {
            isSelected = true;
            flags |= ImGuiTreeNodeFlags_Selected;
            break;
        }
    }

    ImGui::PushID(id);
    bool hasChildren = transform.HasChildren();
    if (!hasChildren)
    {
        flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
    }

    {

        if (bool isActive = data.GetActive(); !isActive)
        {
            color = style.Colors[ImGuiCol_TextDisabled];
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);

        bool node_open =
            ImGui::TreeNodeEx((void *)(intptr_t)id, flags, std::format("{} {}", ICON_FA_CUBE, data.GetName()).c_str());
        DragDrop(transform);
        if (ImGui::IsItemHovered())
        {

            if (ImGui::IsItemClicked() && !isSelected || ImGui::IsItemJustReleased())
            {
                Editor::Get().m_Callbacks[EditorCallback::ObjectSelected].Invoke();

                auto &refSelected = Editor::GetSelectedGameObjects();
                !Input::IsKeyHeld(Keys::SHIFT) ? refSelected.clear() : __nop();
                refSelected.push_back(Scene::ActiveManager().GetGameObject(id));
            }
        }
        PopupMenu(id);

        if (hasChildren)
        {
            for (auto const &child : transform.GetAllDirectChildren())
            {
                RenderNode(child);
            }
        }

        if (node_open)
        {
            ImGui::TreePop();
        }
        ImGui::PopStyleColor();
    }
    ImGui::PopID();
}

inline void Hierarchy::DragDrop(Transform &transform)
{
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
        {
            // TODO do we want all selected? We can guarantee that the payload is one of them
            const auto &list = Editor::Get().GetSelectedGameObjects();
            if (!list.empty())
            {
                for (auto &obj : list)
                {
                    if (!obj.IsValid() && obj.GetID() != transform.GetOwner())
                    {
                        Logger::Warn("Invalid GameObject in hierachy_dragdrop");
                        return;
                    }

                    obj.transform().SetParent(transform);
                }
            }
            else
            {
                GameObject obj = *(GameObject *)payload->Data;
                if (!obj.IsValid() && obj.GetID() != transform.GetOwner())
                {
                    Logger::Warn("Invalid GameObject in hierachy_dragdrop");
                    return;
                }

                obj.transform().SetParent(transform);
            }
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        auto obj = transform.GetGameObject();
        auto objCopy = GameObject(obj);
        ImGui::SetDragDropPayload("HIERARCHY_NODE", &objCopy, sizeof(GameObject));

        ImGui::Text(obj.GetName().c_str());

        if (ImGui::BeginTooltip())
        {
            ImGui::Text(obj.GetName().c_str());
            ImGui::EndTooltip();
        }
        ImGui::EndDragDropSource();
    }
}

void Hierarchy::RenderImGUi()
{
    OPTICK_EVENT();
    const auto &gObjList = Scene::ActiveManager().GetAllGameObjects();
    ImGui::Begin("Hierarchy", &m_KeepWindow);

    static char buf[128] = "";
    {
        ImGui::BeginColumns("HierachySearchSpace", 2);
        ImGui::SetColumnWidth(0, 50);
        ImGui::SetColumnWidth(1, 200);
        ImGui::Button("+");
        ImGui::NextColumn();

        ImGui::InputText("Search", buf, IM_ARRAYSIZE(buf));

        ImGui::EndColumns();
    }
    const std::string keyTerm = buf;

    ImGui::Separator();
    ImGui::BeginChild("GameObjectList");

    using localPair = std::pair<std::string, SY::UUID>;
    static std::vector<localPair> sortedList;

    for (const auto &[id, data] : gObjList)
    {
        const auto &transform = Scene::ActiveManager().GetComponent<Transform>(id);
        if (transform.HasParent() || !data.IsVisibleInHierarcy) // We let parent handle the iterating over the children
        {
            continue;
        }

        sortedList.emplace_back(data.Name, id);
    }

    if (!keyTerm.empty())
    {
        // sort the list
        std::ranges::sort(sortedList, [&keyTerm](const localPair &a, const localPair &b) {
            return Levenstein::Distance(a.first, keyTerm) < Levenstein::Distance(b.first, keyTerm);
        });
    }

    for (const auto &transformIndex : sortedList | std::ranges::views::values)
    {
        auto &transform = Scene::ActiveManager().GetComponent<Transform>(transformIndex);
        RenderNode(transform);
    }

    ImGui::EndChild();
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
        {
            const auto &list = Editor::GetSelectedGameObjects();
            if (!list.empty())
            {
                for (auto &obj : list)
                {
                    if (!obj.IsValid())
                    {
                        Logger::Warn("Invalid GameObject in hierachy_dragdrop");
                        return;
                    }

                    obj.transform().Detach();
                }
            }
            else
            {
                GameObject obj = *(GameObject *)payload->Data;
                if (!obj.IsValid())
                {
                    Logger::Warn("Invalid GameObject in hierachy_dragdrop");
                    return;
                }

                obj.transform().Detach();
            }
        }
        ImGui::Text("Remove parent");
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
    sortedList.clear();
}
