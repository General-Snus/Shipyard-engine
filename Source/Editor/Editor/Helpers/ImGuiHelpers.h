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

	bool BeginMainMenuBar(int barNumber);

	bool ToggleButton(const char* str_id, bool* v);

	Vector2f CursorPositionInWindow();

	void Image(::Texture& aTexture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	void Image(std::shared_ptr<TextureHolder> aTexture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	bool ImageButton(const char* strId, std::shared_ptr<TextureHolder> aTexture, const ImVec2& image_size,
		ImGuiButtonFlags flags = 0, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool ImageButton(const char* strId, ::Texture& aTexture, const ImVec2& image_size, ImGuiButtonFlags flags = 0,
		const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

#pragma warning( push )
#pragma warning( disable : 4505)
	bool IsItemJustReleased();

	bool IsItemJustActivated();
	// Your function
#pragma warning( pop ) 

	void TextCentered(std::string text);
	void TextCentered(const char* text);
	void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
	ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);
	void LoadMarkdownFonts(ImFont* path, ImFont* boldPath, float fontSize_ = 16.0f);
	void Markdown(const std::string& markdown_);


} // namespace ImGui
