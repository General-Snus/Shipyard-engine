#pragma once
#include "../../../DirectX/Shipyard/GpuResource.h"
#include "../../../DirectX/Shipyard/Texture.h"
#include "../../../Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "imgui.h"

namespace ImGui
{
	inline void Image(::Texture& aTexture,const ImVec2& image_size)
	{
		if (const auto id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
		{
			Image(reinterpret_cast<ImTextureID>(id),image_size);
		}
	}

	inline void Image(std::shared_ptr<TextureHolder> aTexture,const ImVec2& image_size)
	{
		if (const auto id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
		{
			Image(reinterpret_cast<ImTextureID>(id),image_size);
		}
	}
}
