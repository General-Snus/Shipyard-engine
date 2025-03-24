#include "../ImageViewer.h"

#include <DirectX/DX12/Graphics/Resources/Texture.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>

#include <Tools/ImGui/imgui_tex_inspect.h>
#include <Tools/ImGui/imgui_tex_inspect_demo.h>
ImageViewer::ImageViewer(std::shared_ptr<Texture> aTexture) : texture(aTexture) {}
ImageViewer::ImageViewer(std::shared_ptr<TextureHolder> aTexture) : texture(aTexture->GetRawTexture()) {}

void ImageViewer::RenderImGUi() {

    const auto windowName =std::format("Image Viewer##{}", uniqueID);
    ImGui::Begin(windowName.c_str(), &m_KeepWindow);
	using namespace ImGuiTexInspect;
	static bool flipX = false;
	static bool flipY = false;
	static bool showWrap = false;
	static bool forceNearestTexel = false;
	static int selectedTexture= false;

    ImGuiTexInspect::InspectorFlags flags = 0;
	if(flipX)  flags |= ImGuiTexInspect::InspectorFlags_FlipX;
	if(flipY) flags |= ImGuiTexInspect::InspectorFlags_FlipY;
    if (showWrap) flags |= ImGuiTexInspect::InspectorFlags_ShowWrap;
    if (!forceNearestTexel) flags |= ImGuiTexInspect::InspectorFlags_NoForceFilterNearest;

	auto v2 = ImVec2((float)texture->GetResolution().x,(float)texture->GetResolution().y); 
    if (ImGuiTexInspect::BeginInspectorPanel("##ColorFilters", texture.get(), v2, flags)) {
        ImGuiTexInspect::DrawAnnotations(ImGuiTexInspect::ValueText(ImGuiTexInspect::ValueText::BytesDec));
    }
	ImGuiTexInspect::EndInspectorPanel();
     

	ImGuiTexInspect::DrawColorChannelSelector();
	ImGui::SameLine(200);
	ImGuiTexInspect::DrawGridEditor();

	ImGui::Separator();

	ImGui::Checkbox("Flip X",&flipX);
	ImGui::Checkbox("Flip Y",&flipY);


    ImGuiTexInspect::DrawAlphaModeSelector();

    ImGui::BeginGroup();
    ImGui::Text("Colour Matrix Editor:");
    // Draw Matrix editor to allow user to manipulate the ColorMatrix
    ImGuiTexInspect::DrawColorMatrixEditor();
    ImGui::EndGroup();

    ImGui::SameLine();

    // Provide some presets that can be used to set the ColorMatrix for example purposes
    ImGui::BeginGroup();
    ImGui::PushItemWidth(200);
    ImGui::Indent(50);
    const ImVec2 buttonSize = ImVec2(160, 0);
    ImGui::Text("Example Presets:");
    // clang-format off
    if (ImGui::Button("Negative", buttonSize))
    {
        // Matrix which inverts each of the red, green, blue channels and leaves Alpha untouched
        float matrix[] = { -1.000f,  0.000f,  0.000f,  0.000f,
                           0.000f, -1.000f,  0.000f,  0.000f,
                           0.000f,  0.000f, -1.000f,  0.000f,
                           0.000f,  0.000f,  0.000f,  1.000f };

        float colorOffset[] = { 1, 1, 1, 0 };
        ImGuiTexInspect::CurrentInspector_SetColorMatrix(matrix, colorOffset);
    }
    if (ImGui::Button("Swap Red & Blue", buttonSize))
    {
        // Matrix which swaps red and blue channels but leaves green and alpha untouched
        float matrix[] = { 0.000f,  0.000f,  1.000f,  0.000f,
                           0.000f,  1.000f,  0.000f,  0.000f,
                           1.000f,  0.000f,  0.000f,  0.000f,
                           0.000f,  0.000f,  0.000f,  1.000f };
        float colorOffset[] = { 0, 0, 0, 0 };
        ImGuiTexInspect::CurrentInspector_SetColorMatrix(matrix, colorOffset);
    }
    if (ImGui::Button("Alpha", buttonSize))
    {
        // Red, green and blue channels are set based on alpha value so that alpha = 1 shows as white. 
        // output alpha is set to 1
        float highlightTransparencyMatrix[] = { 0.000f, 0.000f, 0.000f, 0.000f,
                                               0.000f, 0.000f, 0.000f, 0.000f,
                                               0.000f, 0.000f, 0.000f, 0.000f,
                                               1.000,  1.000,  1.000,  1.000f };
        float highlightTransparencyOffset[] = { 0, 0, 0, 1 };
        ImGuiTexInspect::CurrentInspector_SetColorMatrix(highlightTransparencyMatrix, highlightTransparencyOffset);
    }
    if (ImGui::Button("Transparency", buttonSize))
    {
        // Red, green and blue channels are scaled by 0.1f. Low alpha values are shown as magenta
        float highlightTransparencyMatrix[] = { 0.100f,  0.100f,  0.100f,  0.000f,
                                               0.100f,  0.100f,  0.100f,  0.000f,
                                               0.100f,  0.100f,  0.100f,  0.000f,
                                              -1.000f,  0.000f, -1.000f,  0.000f };
        float highlightTransparencyOffset[] = { 1, 0, 1, 1 };
        ImGuiTexInspect::CurrentInspector_SetColorMatrix(highlightTransparencyMatrix, highlightTransparencyOffset);
    }
    if (ImGui::Button("Default", buttonSize))
    {
        // Default "identity" matrix that doesn't modify colors at all
        float matrix[] = { 1.000f, 0.000f, 0.000f, 0.000f,
                          0.000f, 1.000f, 0.000f, 0.000f,
                          0.000f, 0.000f, 1.000f, 0.000f,
                          0.000f, 0.000f, 0.000f, 1.000f };

        float colorOffset[] = { 0, 0, 0, 0 };
        ImGuiTexInspect::CurrentInspector_SetColorMatrix(matrix, colorOffset);
    }
    // clang-format on
    ImGui::PopItemWidth();
    ImGui::EndGroup();
    ImGui::End();
}
