#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include "TextureAsset.h"

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath,eTextureType atextureType) : AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = atextureType;

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
	}
	if(!RHI::LoadTexture(RawTexture.get(),AssetPath.wstring()))
	{
		if(GraphicsEngine::Get().GetDefaultTexture()->GetRawTexture().get() != nullptr)
		{
			RawTexture = GraphicsEngine::Get().GetDefaultTexture()->GetRawTexture();
			return;
		}
		std::cout << "Error: Default texture was not found" << " \n";
	}
	isLoadedComplete = true;
}

TextureHolder::TextureHolder(const std::filesystem::path& aFilePath) : RawTexture(), AssetBase(aFilePath)
{
	RawTexture = std::make_shared<Texture>();
	this->textureType = eTextureType::ColorMap;

}
  