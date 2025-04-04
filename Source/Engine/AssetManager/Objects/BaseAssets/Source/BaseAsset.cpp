#include "AssetManager.pch.h"

#include "../BaseAsset.h"
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include "Engine/GraphicsEngine/Renderer.h"
#include "Tools/ImGui/ImGuiHelpers.hpp"

AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath), isLoadedComplete(false)
{
}

inline bool AssetBase::InspectorView()
{
	OPTICK_EVENT();
	Vector4f color; // Color asset status indicator
	if (isLoadedComplete)
	{
		color = Vector4f(0, 1, 0, 1);
	}
	else if (isBeingLoaded)
	{
		color = Vector4f(1, 1, 0, 1);
	}
	else
	{
		color = Vector4f(1, 0, 0, 1);
	}

	const TypeInfo& typeInfo = this->GetTypeInfo(); // get custom type info
	bool            isOpened = false;

	const float size = ImGui::CalcTextSize("A").y;
	ImGui::Image(RENDERER.GetDefaultTexture(eTextureType::ColorMap), ImVec2(size, size), ImVec2(0, 0),
	             ImVec2(1, 1), color);
	ImGui::SameLine();
	isOpened =
		ImGui::CollapsingHeader(typeInfo.Name().c_str(), ImGuiTreeNodeFlags_OpenOnDoubleClick |
		                        ImGuiTreeNodeFlags_DefaultOpen);
	return isOpened;
}

std::shared_ptr<TextureHolder> AssetBase::GetEditorIcon()
{
	const auto file = ENGINE_RESOURCES.LoadAsset<TextureHolder>("Textures/Widgets/File.png");

	if (file)
	{
		return file;
	}
	return RENDERER.GetDefaultTexture(eTextureType::ColorMap);
}
