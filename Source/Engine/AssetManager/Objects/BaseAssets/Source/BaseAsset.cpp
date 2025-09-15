#include "AssetManager.pch.h"

#include "../BaseAsset.h" 
#include "Engine/GraphicsEngine/Renderer.h"
#include "Engine\AssetManager\Enums.h"
#include "Engine\AssetManager\AssetManager.h"
#include "Objects\BaseAssets\TextureAsset.h"
#include "Optick\include\optick.h"
#include "Tools/Logging/Logging.h"
#include "Editor\Editor\Helpers\ImGuiHelpers.h"
#include "Tools\Utilities\LinearAlgebra\Vector4.hpp"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <format>
#include <memory>


AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath), isLoadedComplete(false)
{
}

const std::filesystem::path& AssetBase::GetAssetPath() const
{
	return AssetPath;
};

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

	const auto& typeInfo = this->GetTypeInfo(); // get custom type info
	bool            isOpened = false;

	const float size = ImGui::CalcTextSize("A").y;
	ImGui::Image(GetRenderer().GetDefaultTexture(eTextureType::ColorMap), ImVec2(size, size), ImVec2(0, 0),
				 ImVec2(1, 1), color);
	ImGui::SameLine();
	isOpened =
		ImGui::CollapsingHeader(typeInfo.Name().c_str(), ImGuiTreeNodeFlags_OpenOnDoubleClick |
								ImGuiTreeNodeFlags_DefaultOpen);
	return isOpened;
}

std::shared_ptr<TextureHolder> AssetBase::GetEditorIcon()
{
	const auto file = GetEngineResources().LoadAsset<TextureHolder>("Textures/Widgets/File.png");

	if (file)
	{
		return file;
	}
	return GetRenderer().GetDefaultTexture(eTextureType::ColorMap);
}

bool AssetBase::InjectIntoScene(std::shared_ptr<Scene> SceneToAddAsset)
{
	SceneToAddAsset;
	LOGGER.Warn(
		std::format("Asset could not be added to scene\nAsset of type {} is not possible to load into scene",
			GetTypeInfo().Name()));
	return false;
}
void AssetBase::WaitForReady(float timeout) const
{
	assert(isBeingLoaded &&
		"You really should already be trying to load the bloody asset if you wait to wait for it to be ready");

	using fsec = std::chrono::duration<float>;
	if (timeout > 0.0f)
	{
		const auto startTime = std::chrono::system_clock::now();
		float      countdown = 0.0f;
		while (countdown > timeout && isLoadedComplete == false)
		{
			auto diff = std::chrono::system_clock::now() - startTime;
			countdown = std::chrono::duration_cast<fsec>(diff).count();
		}
		return;
	}

	while (isLoadedComplete == false)
	{
	}
}
