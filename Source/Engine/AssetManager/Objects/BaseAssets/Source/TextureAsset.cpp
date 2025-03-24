#include "AssetManager.pch.h"

#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Tools/ImGui/ImGuiHelpers.hpp>
#include "DirectX/DX12/Graphics/GPU.h"
#include "DirectX/DX12/Graphics/Resources/Texture.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath, eTextureType aTextureType)
	: AssetBase(aFilePath), textureType(aTextureType)
{
	RawTexture = std::make_shared<Texture>();
}

bool TextureHolder::InspectorView()
{
	if (!AssetBase::InspectorView())
	{
		return false;
	}
	ImGui::TextCentered(magic_enum::enum_name(textureType).data());
	ImGui::Separator();
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 128);
	ImGui::ImageButton(std::format("##{}", AssetPath.string()).c_str(), *RawTexture, {128, 128});
	ImGui::NextColumn();

	ImGui::Text(AssetPath.filename().string().c_str());

	Vector2ui                res;
	uint32_t                 mip;
	DXGI_FORMAT              format{};
	D3D12_RESOURCE_DIMENSION dim{};
	D3D12_RESOURCE_FLAGS     flags{};
	if (RawTexture)
	{
		res = RawTexture->GetResolution();
		format = RawTexture->m_Format;
		if (RawTexture->m_Resource)
		{
			format = RawTexture->m_Resource->GetDesc().Format;
			mip = RawTexture->m_Resource->GetDesc().MipLevels;
			dim = RawTexture->m_Resource->GetDesc().Dimension;
			flags = RawTexture->m_Resource->GetDesc().Flags;
		}
	}
	ImGui::Text(std::format("Resolution: {}x{}", res.x, res.y).c_str());
	ImGui::Text(std::format("Format: {}", magic_enum::enum_name(format).data()).c_str());
	ImGui::Text(std::format("Flags: {}", magic_enum::enum_name(flags).data()).c_str());
	ImGui::Text(std::format("Dimension: {}", magic_enum::enum_name(dim).data()).c_str());
	ImGui::Text(std::format("Mips: {}", mip).c_str());
	ImGui::EndColumns();
	return true;
}

std::shared_ptr<TextureHolder> TextureHolder::GetEditorIcon()
{
	const auto file = ENGINE_RESOURCES.LoadAsset<TextureHolder>(AssetPath, true);

	if (file)
	{
		return file;
	}
	return GraphicsEngineInstance.GetDefaultTexture(eTextureType::ColorMap);
}

void TextureHolder::SetTextureType(eTextureType aTextureType)
{
	RawTexture->isCubeMap = (aTextureType == eTextureType::CubeMap);
	textureType = aTextureType;
}

void TextureHolder::Init()
{
	const auto position = static_cast<int>(AssetPath.filename().string().find_last_of("_"));
	if (position > -1)
	{
		const std::string typeOfTexture = AssetPath.filename().replace_extension("").string().substr(position);

		if (typeOfTexture == "_C" || typeOfTexture == "_c")
		{
			this->textureType = eTextureType::ColorMap;
		}
		if (typeOfTexture == "_N" || typeOfTexture == "_n")
		{
			this->textureType = eTextureType::NormalMap;
		}
		if (typeOfTexture == "_M" || typeOfTexture == "_m")
		{
			this->textureType = eTextureType::MaterialMap;
		}
		if (typeOfTexture == "_FX" || typeOfTexture == "_fx" || typeOfTexture == "_Fx")
		{
			this->textureType = eTextureType::EffectMap;
		}
		if (typeOfTexture == "_P" || typeOfTexture == "_p")
		{
			this->textureType = eTextureType::ParticleMap;
		}
	}

	if (AssetPath.extension() == ".dds" || AssetPath.extension() == ".png" || AssetPath.extension() == ".jpg")
	{
		if (!exists(AssetPath))
		{
			if (!ENGINE_RESOURCES.AdaptPath(AssetPath))
			{
				const std::string msg = "Error: Coulnt load texture at " + AssetPath.string();
				LOGGER.Err(msg);
				if (GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
				{
					RawTexture = GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture();
					isLoadedComplete = true;
					return;
				}
				isLoadedComplete = false;
				return;
			}
		}

		if (!GPUInstance.LoadTexture(RawTexture.get(), AssetPath.wstring()))
		{
			const std::string msg = "Error: Coulnt dds texture at " + AssetPath.string();
			LOGGER.Err(msg);
			if (GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
			{
				RawTexture = GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture();
				isLoadedComplete = true;
				RawTexture->SetView(ViewType::SRV);
				return;
			}
			LOGGER.Err("Error: Default texture was not found");

			isLoadedComplete = false;
			return;
		}

		isLoadedComplete = true;
	}
	else
	{
		const std::string msg = "Error: Coulnt load generic texture at " + AssetPath.string();
		LOGGER.Err(msg);

		// if (GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
		//{
		//	RawTexture = GraphicsEngineInstance.GetDefaultTexture(this->textureType)->GetRawTexture();
		//	isLoadedComplete = true;
		//	RawTexture->SetView(ViewType::SRV);
		//	return;
		// }
		// Logger.Err("Error: Default texture was not found");
		isBeingLoaded = false;
		isLoadedComplete = false;
	}
}

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = eTextureType::ColorMap;
}
