#pragma once
#include "DirectX/Shipyard/GpuResource.h"
#include "DirectX/Shipyard/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Tools/ImGui/ImGui/imgui.h"
#include <DirectX/Shipyard/Enums.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>

namespace ImGui
{

bool ToggleButton(const char *str_id, bool *v)
{
    bool returnValue = false;
    ImVec4 *colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
    {
        *v = !*v;
        returnValue = true;
    }

    ImGuiContext &gg = *GImGui;
    // float ANIM_SPEED = 0.085f;
    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
    {
        // float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
    }

    if (ImGui::IsItemHovered())
    {
        draw_list->AddRectFilled(
            p, ImVec2(p.x + width, p.y + height),
            ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
    }
    else
    {
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height),
                                 ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)),
                                 height * 0.50f);
    }

    draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius),
                               radius - 1.5f, IM_COL32(255, 255, 255, 255));

    return returnValue;
}

inline void Image(::Texture &aTexture, const ImVec2 &image_size, const ImVec2 &uv0 = ImVec2(0, 0),
                  const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
                  const ImVec4 &border_col = ImVec4(0, 0, 0, 0))
{
    if (const auto id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
    {
        Image(reinterpret_cast<ImTextureID>(id), image_size, uv0, uv1, tint_col, border_col);
    }
}

inline void Image(std::shared_ptr<TextureHolder> aTexture, const ImVec2 &image_size, const ImVec2 &uv0 = ImVec2(0, 0),
                  const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
                  const ImVec4 &border_col = ImVec4(0, 0, 0, 0))
{
    if (const auto id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
    {
        Image(reinterpret_cast<ImTextureID>(id), image_size, uv0, uv1, tint_col, border_col);
    }
}
inline bool ImageButton(const char *strId, std::shared_ptr<TextureHolder> aTexture, const ImVec2 &image_size,
                        ImGuiButtonFlags flags = 0, const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
                        const ImVec4 &bg_col = ImVec4(0, 0, 0, 0), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1))
{
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    if (const auto id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
    {
        return ImageButtonEx(window->GetID(strId), reinterpret_cast<ImTextureID>(id), image_size, uv0, uv1, bg_col,
                             tint_col, flags);
    }
    else
    {
        auto newId = GraphicsEngine::Get()
                         .GetDefaultTexture(eTextureType::ColorMap)
                         ->GetRawTexture()
                         ->GetHandle(ViewType::SRV)
                         .gpuPtr.ptr;
        return ImageButtonEx(window->GetID(strId), reinterpret_cast<ImTextureID>(newId), image_size, uv0, uv1, bg_col,
                             tint_col, flags);
    }
    return false;
}

inline bool ImageButton(const char *strId, ::Texture &aTexture, const ImVec2 &image_size, ImGuiButtonFlags flags = 0,
                        const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
                        const ImVec4 &bg_col = ImVec4(0, 0, 0, 0), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1))
{
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    if (const auto id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
    {
        return ImageButtonEx(window->GetID(strId), reinterpret_cast<ImTextureID>(id), image_size, uv0, uv1, bg_col,
                             tint_col, flags);
    }
    else
    {
        auto newId = GraphicsEngine::Get()
                         .GetDefaultTexture(eTextureType::ColorMap)
                         ->GetRawTexture()
                         ->GetHandle(ViewType::SRV)
                         .gpuPtr.ptr;
        return ImageButtonEx(window->GetID(strId), reinterpret_cast<ImTextureID>(newId), image_size, uv0, uv1, bg_col,
                             tint_col, flags);
    }
}

inline void TextCentered(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

inline void TextCentered(const char *text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text);
}

static bool IsItemJustReleased()
{
    return IsItemDeactivated() && !ImGui::IsItemActive();
}
static bool IsItemJustActivated()
{
    return !IsItemDeactivated() && ImGui::IsItemActive();
}
} // namespace ImGui
