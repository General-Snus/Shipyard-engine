#pragma once
#include <Tools\Utilities\System\SingletonTemplate.h>
#include "Tools/ImGui/imgui.h"
#include "Tools/ImGui/imgui_markdown.h"

class Texture;
class TextureHolder;

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

	void InitializeOnNewContext(ImGuiContextHolder& context);

	inline bool BeginMainMenuBar(int barNumber);

	inline bool ToggleButton(const char* str_id, bool* v);

	inline Vector2f CursorPositionInWindow();

	inline void Image(::Texture& aTexture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	inline void Image(std::shared_ptr<TextureHolder> aTexture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	inline bool ImageButton(const char* strId, std::shared_ptr<TextureHolder> aTexture, const ImVec2& image_size,
		ImGuiButtonFlags flags = 0, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	inline bool ImageButton(const char* strId, ::Texture& aTexture, const ImVec2& image_size, ImGuiButtonFlags flags = 0,
		const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

#pragma warning( push )
#pragma warning( disable : 4505)
	inline bool IsItemJustReleased();

	inline bool IsItemJustActivated(); 
	// Your function
#pragma warning( pop ) 

	inline void TextCentered(std::string text);
	inline void TextCentered(const char* text);
	inline void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
	inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);
	inline void LoadMarkdownFonts(ImFont* path, ImFont* boldPath, float fontSize_ = 16.0f);
	inline void Markdown(const std::string& markdown_);


} // namespace ImGui
