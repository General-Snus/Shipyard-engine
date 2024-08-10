#pragma once
#include "DirectX/Shipyard/GpuResource.h"
#include "DirectX/Shipyard/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Tools/ImGui/ImGui/imgui.h"
#include <Engine/GraphicsEngine/GraphicsEngine.h>

namespace ImGui
{
inline void Image(::Texture &aTexture, const ImVec2 &image_size)
{
    if (const auto id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
    {
        Image(reinterpret_cast<ImTextureID>(id), image_size);
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
    return false;
}

inline void TextCentered(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

inline void TextCentered(const char* text)
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
