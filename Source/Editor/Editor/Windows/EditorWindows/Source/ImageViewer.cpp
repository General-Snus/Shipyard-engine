#include "../ImageViewer.h"

#include <DirectX/DX12/Graphics/Texture.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>

#include <Tools/ImGui/imgui_tex_inspect.h>
#include <Tools/ImGui/imgui_tex_inspect_demo.h>
ImageViewer::ImageViewer(std::shared_ptr<Texture> aTexture) : texture(aTexture) {}
ImageViewer::ImageViewer(std::shared_ptr<TextureHolder> aTexture) : texture(aTexture->GetRawTexture()) {}

void ImageViewer::RenderImGUi() {

	using namespace ImGuiTexInspect;
	static bool flipX = false;
	static bool flipY = false;
	static int selectedTexture= false;

	ImGuiTexInspect::InspectorFlags flags = 0;
	if(flipX)  flags |= ImGuiTexInspect::InspectorFlags_FlipX;
	if(flipY) flags |= ImGuiTexInspect::InspectorFlags_FlipY;

	auto v2 = ImVec2((float)texture->GetResolution().x,(float)texture->GetResolution().y);
	const ImTextureID id = texture->GetHandle(ViewType::SRV).gpuPtr.ptr;
	if(ImGuiTexInspect::BeginInspectorPanel("##ColorFilters",id,v2,flags)) {
 		ImGuiTexInspect::DrawAnnotations(ImGuiTexInspect::ValueText(ImGuiTexInspect::ValueText::BytesDec));
	}

	ImGuiTexInspect::EndInspectorPanel();

 	ImGui::BeginChild("Controls",ImVec2(600,100));
	ImGui::TextWrapped("Basics:");
	ImGui::BulletText("Use mouse wheel to zoom in and out.  Click and drag to pan.");
	ImGui::BulletText("Use the demo select buttons at the top of the window to explore");
	ImGui::BulletText("Use the controls below to change basic color filtering options");
	ImGui::EndChild();


	/* DrawColorChannelSelector & DrawGridEditor are convenience functions that
	 * draw ImGui controls to manipulate config of the most recently drawn
	 * texture inspector
	 **/
	ImGuiTexInspect::DrawColorChannelSelector();
	ImGui::SameLine(200);
	ImGuiTexInspect::DrawGridEditor();

	ImGui::Separator();

	ImGui::Checkbox("Flip X",&flipX);
	ImGui::Checkbox("Flip Y",&flipY);

}
