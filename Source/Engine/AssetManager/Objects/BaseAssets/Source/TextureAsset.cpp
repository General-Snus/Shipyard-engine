#include "Engine/AssetManager/AssetManager.pch.h"  

#define STB_IMAGE_IMPLEMENTATION	
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
 
#include "DirectX/Shipyard/Texture.h" 
#include "DirectX/Shipyard/GPU.h" 
#include "Engine/GraphicsEngine/GraphicsEngine.h" 

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath,eTextureType aTextureType) : AssetBase(aFilePath),textureType(aTextureType)
{
	RawTexture = std::make_shared<Texture>();
}

void TextureHolder::InspectorView()
{
	AssetBase::InspectorView(); 
}

void TextureHolder::SetTextureType(eTextureType aTextureType)
{
	RawTexture->isCubeMap = (aTextureType == eTextureType::CubeMap);
	textureType = aTextureType;
}

void TextureHolder::Init()
{
	const int position = (int)AssetPath.filename().string().find_last_of("_");
	if (position > -1)
	{
		const std::string  typeOfTexture = AssetPath.filename().replace_extension("").string().substr(position);

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

	if (AssetPath.extension() == ".dds" || AssetPath.extension() == ".png")
	{
		if (!std::filesystem::exists(AssetPath))
		{
			if (!AssetManager::Get().AdaptPath(AssetPath))
			{
				const std::string msg = "Error: Coulnt load texture at " + AssetPath.string();
				Logger::Err(msg);
				if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
				{
					RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
					isLoadedComplete = true;
					return;
				}
				isLoadedComplete = false;
				return;
			}
		}

		if (!GPU::LoadTexture(RawTexture.get(),AssetPath.wstring()))
		{
			const std::string msg = "Error: Coulnt dds texture at " + AssetPath.string();
			Logger::Err(msg);
			if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
			{
				RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
				isLoadedComplete = true;
				RawTexture->SetView(ViewType::SRV);
				return;
			} 
			Logger::Err("Error: Default texture was not found");

			isLoadedComplete = false;
			return;
		}

		isLoadedComplete = true;
	}
	else
	{
		const std::string msg = "Error: Coulnt load generic texture at " + AssetPath.string();
		Logger::Err(msg);

		//if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
		//{
		//	RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
		//	isLoadedComplete = true;
		//	RawTexture->SetView(ViewType::SRV);
		//	return;
		//}
		//Logger::Err("Error: Default texture was not found");
		isBeingLoaded = false;
		isLoadedComplete = false;
		return;
	}
}

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = eTextureType::ColorMap;
}
