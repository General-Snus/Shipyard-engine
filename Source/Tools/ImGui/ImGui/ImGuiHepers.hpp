#pragma once
#include "DirectX/Shipyard/GpuResource.h"
#include "DirectX/Shipyard/Texture.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Tools/ImGui/ImGui/imgui.h"

namespace ImGui
{
	inline void Image(::Texture& aTexture,const ImVec2& image_size)
	{
		if (const auto id = aTexture.GetHandle(ViewType::SRV).gpuPtr.ptr)
		{
			Image(reinterpret_cast<ImTextureID>(id),image_size);
		}
	}

	inline void Image(std::shared_ptr<TextureHolder> aTexture,const ImVec2& image_size,const ImVec2& uv0 = ImVec2(0,0),const ImVec2& uv1 = ImVec2(1,1),const ImVec4& tint_col = ImVec4(1,1,1,1),const ImVec4& border_col = ImVec4(0,0,0,0))
	{
		if (const auto id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
		{
			Image(reinterpret_cast<ImTextureID>(id),image_size,uv0,uv1,tint_col,border_col);
		}
	}
	inline void ImageButton(const char* fileName, std::shared_ptr<TextureHolder> aTexture,const ImVec2& image_size,const ImVec2& uv0 = ImVec2(0,0),const ImVec2& uv1 = ImVec2(1,1),const ImVec4& bg_col = ImVec4(0,0,0,0),const ImVec4& tint_col = ImVec4(1,1,1,1))
	{
		if (const auto id = aTexture->GetRawTexture()->GetHandle(ViewType::SRV).gpuPtr.ptr)
		{
			ImageButton(fileName,reinterpret_cast<ImTextureID>(id),image_size,uv0,uv1,bg_col,tint_col);
		}
	} 
}
