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

bool TextureHolder::LoadPngTexture(Texture* outTexture,const std::wstring& aFileName)
{
	outTexture->myName = aFileName;
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;

	auto res = DirectX::CreateWICTextureFromFile(
		RHI::Device.Get(),
		aFileName.c_str(),
		outTexture->myTexture.GetAddressOf(),
		outTexture->mySRV.GetAddressOf());
	if(res < 0)
	{
		return false;
	}

	res = RHI::Device.Get()->CreateShaderResourceView(outTexture->myTexture.Get(),nullptr,outTexture->mySRV.GetAddressOf());
	if(res < 0)
	{
		return false;
	}

	std::wstring textureName = aFileName;
	if(const size_t pos = textureName.find_last_of(L'\\'); pos != std::wstring::npos)
	{
		textureName = textureName.substr(pos + 1);
	}

	textureName = textureName.substr(0,textureName.size() - 4);
	outTexture->myName = textureName;

	return true;
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
			if(!AssetManager::Get().AdaptPath(AssetPath))
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

		if(!LoadPngTexture(RawTexture.get(),AssetPath))
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
		if(!RawTexture->mySRV)
		{
			std::cout << "Error: Default texture was not found" << " \n";
			isLoadedComplete = false;
			return;
		}
		isLoadedComplete = true;
		return;
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
}

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = eTextureType::ColorMap;
}
