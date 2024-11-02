#pragma once
#include "DirectX/DX12/Graphics/GpuResource.h"
#include "DirectX/DX12/Graphics/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Tools/ImGui/imgui.h"
#include <DirectX/DX12/Graphics/Enums.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>

namespace ImGui
{
class ImGuiContextHolder : public Singleton
{
  public:
    ImGuiContext *ctx;
    ImGuiMemAllocFunc v1;
    ImGuiMemFreeFunc v2;
    void *v3;
};

inline void InitializeOnNewContext(ImGuiContextHolder &context)
{
    ImGui::SetCurrentContext(context.ctx);
    ImGui::SetAllocatorFunctions(context.v1, context.v2, context.v3);
}

inline bool BeginMainMenuBar(int barNumber)
{
    if (barNumber == 0)
    {
        return BeginMainMenuBar();
    }

    ImGuiContext &g = *GImGui;
    ImGuiViewportP *viewport = (ImGuiViewportP *)(void *)GetMainViewport();

    // Notify of viewport change so GetFrameHeight() can be accurate in case of
    // DPI change
    SetCurrentViewport(NULL, viewport);

    // For the main menu bar, which cannot be moved, we honor
    // g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
    // FIXME: This could be generalized as an opt-in way to clamp
    // window->DC.CursorStartPos to avoid SafeArea?
    // FIXME: Consider removing support for safe area down the line... it's messy.
    // Nowadays consoles have support for TV calibration in OS settings.
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(
        g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    float height = GetFrameHeight();
    bool is_open = BeginViewportSideBar(std::format("##MainMenuBar{}", barNumber).c_str(), viewport, ImGuiDir_Up,
                                        height, window_flags);
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);

    if (is_open)
        BeginMenuBar();
    else
        End();
    return is_open;
};

inline bool ToggleButton(const char *str_id, bool *v)
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
        auto newId = GraphicsEngineInstance.GetDefaultTexture(eTextureType::ColorMap)
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
        auto newId = GraphicsEngineInstance.GetDefaultTexture(eTextureType::ColorMap)
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
