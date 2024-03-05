#include "AssetManager.pch.h" 
#include "../TextureAsset.h"

#define STB_IMAGE_IMPLEMENTATION	
#include <Tools/Utilities/Files/Images/stb_image.h>

#include "DirectX/Shipyard/GPU.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h" 

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
#if WorkingOnPngLoading 
bool TextureHolder::LoadPngTexture(Texture* outTexture,const std::filesystem::path& aFileName)
{
	outTexture->myName = aFileName;
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;


	int ImageWidth;
	int ImageHeight;
	int ImageChannels;
	int ImageDesiredChannels = 4;

	unsigned char* ImageData = stbi_load(aFileName.string().c_str(),
		&ImageWidth,
		&ImageHeight,
		&ImageChannels,
		ImageDesiredChannels);
	assert(ImageData);

	int ImagePitch = ImageWidth * 4;


	D3D11_TEXTURE2D_DESC ImageTextureDesc = {};
	ImageTextureDesc.Width = ImageWidth;
	ImageTextureDesc.Height = ImageHeight;
	ImageTextureDesc.MipLevels = 1;
	ImageTextureDesc.ArraySize = 1;
	ImageTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ImageTextureDesc.SampleDesc.Count = 1;
	ImageTextureDesc.SampleDesc.Quality = 0;
	ImageTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA ImageSubresourceData = {};

	ImageSubresourceData.pSysMem = ImageData;
	ImageSubresourceData.SysMemPitch = ImagePitch;
	ID3D11Texture2D* ImageTexture;
	auto Result = RHI::Device->CreateTexture2D(&ImageTextureDesc,
		&ImageSubresourceData,
		&ImageTexture
	);
	assert(SUCCEEDED(Result));

	outTexture->myTexture = ImageTexture;
	free(ImageData);

	// Shader resource view 
	Result = RHI::Device->CreateShaderResourceView(
		outTexture->myTexture.Get(),
		nullptr,
		outTexture->mySRV.GetAddressOf()
	);
	assert(SUCCEEDED(Result));


	//auto res = DirectX::CreateWICTextureFromFile(
	//	RHI::Device.Get(),
	//	aFileName.c_str(),
	//	outTexture->myTexture.GetAddressOf(),
	//	outTexture->mySRV.GetAddressOf());


	/*auto res = RHI::Device.Get()->CreateShaderResourceView(outTexture->myTexture.Get(),nullptr,outTexture->mySRV.GetAddressOf());
	if(res < 0)
	{
		return false;
	}*/

	std::wstring textureName = aFileName;
	if (const size_t pos = textureName.find_last_of(L'\\'); pos != std::wstring::npos)
	{
		textureName = textureName.substr(pos + 1);
	}

	textureName = textureName.substr(0,textureName.size() - 4);
	outTexture->myName = textureName;

	return true;
}
#endif

void TextureHolder::Init()
{
	int position = (int)AssetPath.filename().string().find_last_of("_");
	if (position > -1)
	{
		std::string  typeOfTexture = AssetPath.filename().replace_extension("").string().substr(position);

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

#if WorkingOnPngLoading
	if (AssetPath.extension() == ".png")
	{
		if (!std::filesystem::exists(AssetPath))
		{
			if (!AssetManager::Get().AdaptPath(AssetPath))
			{
				std::string msg = "Error: Coulnt load texture at " + AssetPath.string();
				std::cout << msg << " \n";
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
		if (!LoadPngTexture(RawTexture.get(),AssetPath))
		{
			std::string msg = "Error: Coulnt dds texture at " + AssetPath.string();
			std::cout << msg << " \n";
			if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
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
#endif
	else if (AssetPath.extension() == ".dds")
	{
		if (!GPU::LoadTexture(RawTexture.get(),AssetPath.wstring()))
		{
			std::string msg = "Error: Coulnt dds texture at " + AssetPath.string();
			std::cout << msg << " \n";
			if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
			{
				RawTexture = GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture();
				isLoadedComplete = true;
				return;
			}
			std::cout << "Error: Default texture was not found" << " \n";
			isLoadedComplete = false;
			return;
		}
		//isLoadedComplete = true;
	}
	else
	{
		std::string msg = "Error: Coulnt load generic texture at " + AssetPath.string();
		std::cout << msg << " \n";
		if (GraphicsEngine::Get().GetDefaultTexture(this->textureType)->GetRawTexture().get() != nullptr)
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
