#pragma once
#include "DirectX/DX12/Graphics/GpuResource.h"
#include "DirectX/DX12/Graphics/Resources/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Tools/ImGui/imgui.h"
#include "Tools/ImGui/imgui_markdown.h" 
#include "Tools/Utilities/Input/Input.hpp" 

#include <DirectX/DX12/Graphics/Enums.h>
#include <Engine/GraphicsEngine/Renderer.h>

#include "Shellapi.h"

namespace ImGui
{
    class ImGuiContextHolder : public Singleton
    {
    public:
        ImGuiContext* ctx;
        ImGuiMemAllocFunc v1;
        ImGuiMemFreeFunc v2;
        void* v3;
    };

    inline void InitializeOnNewContext(ImGuiContextHolder& context)
    {
        OPTICK_EVENT();
        ImGui::SetCurrentContext(context.ctx);
        ImGui::SetAllocatorFunctions(context.v1,context.v2,context.v3);
    }

    inline bool BeginMainMenuBar(int barNumber)
    {
        OPTICK_EVENT();
        if(barNumber == 0)
        {
            return BeginMainMenuBar();
        }

        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)GetMainViewport();

        // Notify of viewport change so GetFrameHeight() can be accurate in case of
        // DPI change
        //SetCurrentViewport(NULL,viewport);

        // For the main menu bar, which cannot be moved, we honor
        // g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
        // FIXME: This could be generalized as an opt-in way to clamp
        // window->DC.CursorStartPos to avoid SafeArea?
        // FIXME: Consider removing support for safe area down the line... it's messy.
        // Nowadays consoles have support for TV calibration in OS settings.
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(
            g.Style.DisplaySafeAreaPadding.x,ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y,0.0f));
        const ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        const float height = GetFrameHeight();
        const bool is_open = BeginViewportSideBar(std::format("##MainMenuBar{}",barNumber).c_str(),viewport,ImGuiDir_Up,
                                                  height,window_flags);
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f,0.0f);

        if(is_open)
            BeginMenuBar();
        else
            End();
        return is_open;
    };

    inline bool ToggleButton(const char* str_id,bool* v)
    {
        OPTICK_EVENT();
        bool returnValue = false;
        const ImVec4* colors = ImGui::GetStyle().Colors;
        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float height = ImGui::GetFrameHeight();
        const float width = height * 1.55f;
        const float radius = height * 0.50f;

        ImGui::InvisibleButton(str_id,ImVec2(width,height));
        if(ImGui::IsItemClicked())
        {
            *v = !*v;
            returnValue = true;
        }

        const ImGuiContext& gg = *GImGui;
        // float ANIM_SPEED = 0.085f;
        if(gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
        {
            // float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
        }

        if(ImGui::IsItemHovered())
        {
            draw_list->AddRectFilled(
                p,ImVec2(p.x + width,p.y + height),
                ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f,0.78f,0.78f,1.0f)),height * 0.5f);
        }
        else
        {
            draw_list->AddRectFilled(p,ImVec2(p.x + width,p.y + height),
                                     ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f,0.85f,0.85f,1.0f)),
                                     height * 0.50f);
        }

        draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f),p.y + radius),
                                   radius - 1.5f,IM_COL32(255,255,255,255));

        return returnValue;
    }

    inline Vector2f CursorPositionInWindow()
    {
        const auto mp = Input.GetMousePosition();
        const auto imgui_cursor = ImGui::GetWindowPos();
        const auto screen_size = Vector2f(ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);
        const auto relativeCursorPos = Vector2f(
            std::clamp(mp.x - imgui_cursor.x,0.f,screen_size.x),
            std::clamp(mp.y - imgui_cursor.y,0.f,screen_size.y)
        );

        const auto relativeNDC = Vector2f(
            -1.0f + 2.0f * (relativeCursorPos.x / screen_size.x),
            -1.0f + 2.0f * (1.0f - (relativeCursorPos.y / screen_size.y))
        );

        return {std::clamp(relativeNDC.x, -1.0f, 1.0f),
                std::clamp(relativeNDC.y, -1.0f, 1.0f)};
    }

    inline void Image(::Texture& aTexture,const ImVec2& image_size,const ImVec2& uv0 = ImVec2(0,0),
                      const ImVec2& uv1 = ImVec2(1,1),const ImVec4& tint_col = ImVec4(1,1,1,1),
                      const ImVec4& border_col = ImVec4(0,0,0,0))
    {
        OPTICK_EVENT();
        if(const ImTextureID id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
        {
            Image(id,image_size,uv0,uv1,tint_col,border_col);
        }
    }

    inline void Image(std::shared_ptr<TextureHolder> aTexture,const ImVec2& image_size,const ImVec2& uv0 = ImVec2(0,0),
                      const ImVec2& uv1 = ImVec2(1,1),const ImVec4& tint_col = ImVec4(1,1,1,1),
                      const ImVec4& border_col = ImVec4(0,0,0,0))
    {
        OPTICK_EVENT();
        if(const ImTextureID id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
        {
            Image(id,image_size,uv0,uv1,tint_col,border_col);
        }
    }

    inline bool ImageButton(const char* strId,std::shared_ptr<TextureHolder> aTexture,const ImVec2& image_size,
                            ImGuiButtonFlags flags = 0,const ImVec2& uv0 = ImVec2(0,0),const ImVec2& uv1 = ImVec2(1,1),
                            const ImVec4& bg_col = ImVec4(0,0,0,0),const ImVec4& tint_col = ImVec4(1,1,1,1))
    {

        OPTICK_EVENT();
        const ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if(window->SkipItems)
        {
            return false;
        }

        const auto windowsID = window->GetID(strId);
        if(const ImTextureID id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
        {
            return ImageButtonEx(windowsID,id,image_size,uv0,uv1,bg_col,
                                 tint_col,flags);
        }
        else
        {
            const ImTextureID newId = RENDERER.GetDefaultTexture(eTextureType::ColorMap)
                ->GetRawTexture()
                ->GetHandle(ViewType::SRV)
                .gpuPtr.ptr;
            return ImageButtonEx(windowsID,newId,image_size,uv0,uv1,bg_col,
                                 tint_col,flags);
        }
        return false;
    }

    inline bool ImageButton(const char* strId,::Texture& aTexture,const ImVec2& image_size,ImGuiButtonFlags flags = 0,
                            const ImVec2& uv0 = ImVec2(0,0),const ImVec2& uv1 = ImVec2(1,1),
                            const ImVec4& bg_col = ImVec4(0,0,0,0),const ImVec4& tint_col = ImVec4(1,1,1,1))
    {
        OPTICK_EVENT();
        const ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if(window->SkipItems)
        {
            return false;
        }

        const auto winddowID = window->GetID(strId);
        if(const ImTextureID id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
        {
            return ImageButtonEx(winddowID,id,image_size,uv0,uv1,bg_col,
                                 tint_col,flags);
        }
        else
        {
            const auto newId = RENDERER.GetDefaultTexture(eTextureType::ColorMap)
                ->GetRawTexture()
                ->GetHandle(ViewType::SRV)
                .gpuPtr.ptr;
            return ImageButtonEx(winddowID,newId,image_size,uv0,uv1,bg_col,
                                 tint_col,flags);
        }
    }

#pragma warning( push )
#pragma warning( disable : 4505)
    inline static bool IsItemJustReleased()
    {
        OPTICK_EVENT();
        return IsItemDeactivated() && !ImGui::IsItemActive();
    }
    inline  static bool IsItemJustActivated()
    {
        OPTICK_EVENT();
        return !IsItemDeactivated() && ImGui::IsItemActive();
    }
    // Your function
#pragma warning( pop ) 


    //TEXT HELPERS
    inline void TextCentered(std::string text)
    {
        OPTICK_EVENT();
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text.c_str());
    }

    inline void TextCentered(const char* text)
    {
        OPTICK_EVENT();
        const auto windowWidth = ImGui::GetWindowSize().x;
        const auto textWidth = ImGui::CalcTextSize(text).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text);
    }


    inline void LinkCallback(ImGui::MarkdownLinkCallbackData data_)
    {
        std::string url(data_.link,data_.linkLength);
        if(!data_.isImage)
        {
            ShellExecuteA(nullptr,"open",url.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
        }
    }

    inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
    {
        data_;
        // In your application you would load an image based on data_ input. Here we just use the imgui font texture.
        ImTextureID image = ImGui::GetIO().Fonts->TexID;
        // > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
        ImGui::MarkdownImageData imageData;
        imageData.isValid = true;
        imageData.useLinkCallback = false;
        imageData.user_texture_id = image;
        imageData.size = ImVec2(40.0f,20.0f);

        // For image resize when available size.x > image width, add
        ImVec2 const contentSize = ImGui::GetContentRegionAvail();
        if(imageData.size.x > contentSize.x)
        {
            float const ratio = imageData.size.y / imageData.size.x;
            imageData.size.x = contentSize.x;
            imageData.size.y = contentSize.x * ratio;
        }

        return imageData;
    }

    inline void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
    inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);
    inline static MarkdownConfig mdConfig;

    static ImFont* H1 = nullptr;
    static ImFont* H2 = nullptr;
    static ImFont* H3 = nullptr;

    inline void LoadMarkdownFonts(ImFont* path,ImFont* boldPath,float fontSize_ = 16.0f)
    {
        path; fontSize_;
        // Bold headings H2 and H3
        H2 = boldPath;
        H3 = path;
        H1 = boldPath;
    }

    inline void Markdown(const std::string& markdown_)
    {
        // You can make your own Markdown function with your prefered string container and markdown config.
        // > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
        mdConfig.linkCallback = LinkCallback;
        mdConfig.tooltipCallback = NULL;
        mdConfig.imageCallback = ImageCallback;
        mdConfig.linkIcon = "\xef\x83\x81"; // ICON_FA_LINK IM LAZY OK
        mdConfig.headingFormats[0] = {H1, true};
        mdConfig.headingFormats[1] = {H2, true};
        mdConfig.headingFormats[2] = {H3, false};
        mdConfig.userData = NULL; ;
        ImGui::Markdown(markdown_.c_str(),markdown_.length(),mdConfig);
    }
} // namespace ImGui
