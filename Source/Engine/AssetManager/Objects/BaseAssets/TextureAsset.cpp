#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include <Tools/ThirdParty/DirectXTK/WICTextureLoader.h>
#include "TextureAsset.h" 

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath,eTextureType aTextureType) : AssetBase(aFilePath),textureType(aTextureType)
{
	RawTexture = std::make_shared<Texture>();

	/*if(!RHI::LoadTexture(RawTexture.get(),aFilePath.wstring()))
	{
		if(GraphicsEngine::Get().GetDefaultTexture(atextureType) != nullptr)
		{
			RawTexture = GraphicsEngine::Get().GetDefaultTexture(atextureType)->GetRawTexture();
			return;
		}
		std::cout << "Error: Default texture was not found" << " \n";
	}*/
}

void TextureHolder::Init()
{
	int position = (int)AssetPath.filename().string().find_last_of("_");
	if(position > -1)
	{
		std::string  typeOfTexture = AssetPath.filename().replace_extension("").string().substr(position);

		if(typeOfTexture == "_C" || typeOfTexture == "_c")
		{
			this->textureType = eTextureType::ColorMap;
		}
		if(typeOfTexture == "_N" || typeOfTexture == "_n")
		{
			this->textureType = eTextureType::NormalMap;
		}
		if(typeOfTexture == "_M" || typeOfTexture == "_m")
		{
			this->textureType = eTextureType::MaterialMap;
		}
		if(typeOfTexture == "_FX" || typeOfTexture == "_fx" || typeOfTexture == "_Fx")
		{
			this->textureType = eTextureType::EffectMap;
		}
		if(typeOfTexture == "_P" || typeOfTexture == "_p")
		{
			this->textureType = eTextureType::ParticleMap;
		}
	}

	if(AssetPath.extension() == ".png")
	{
		if(!std::filesystem::exists(AssetPath))
		{
			if(!AssetManager::GetInstance().AdaptPath(AssetPath))
			{
				if(GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
				{
					RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
					isLoadedComplete = true;
					return;
				}
				isLoadedComplete = false;
				return;
			}
		}

		if(!DirectX::CreateWICTextureFromFile(
			RHI::Device.Get(),
			AssetPath.wstring().c_str(),
			RawTexture->myTexture.GetAddressOf(),
			RawTexture->mySRV.GetAddressOf()))
		{
			if(GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
			{
				RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
				isLoadedComplete = true;
				return;
			}
			std::cout << "Error: Default texture was not found" << " \n";
			isLoadedComplete = false;
			return;
		}
	}
	else if(AssetPath.extension() == ".dds")
	{
		if(!RHI::LoadTexture(RawTexture.get(),AssetPath.wstring()))
		{
			if(GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
			{
				RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
				isLoadedComplete = true;
				return;
			}
			std::cout << "Error: Default texture was not found" << " \n";
			isLoadedComplete = false;
			return;
		}
	}
	else
	{
		if(GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
		{
			RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
			isLoadedComplete = true;
			return;
		}
		std::cout << "Error: Default texture was not found" << " \n";
		isLoadedComplete = false;
		return;
	}
	isLoadedComplete = true;
}

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = eTextureType::ColorMap;
}
