#pragma once 
#include "BaseAsset.h"
#include "DirectX/Shipyard/Texture.h"
class Texture;

class TextureHolder : public AssetBase
{
	friend class Material;
public:
	MYLIB_REFLECTABLE();
	void Init() override;
	TextureHolder(const std::filesystem::path& aFilePath);
	TextureHolder(const std::filesystem::path& aFilePath,eTextureType atextureType);
	std::shared_ptr<Texture> GetRawTexture() const { return RawTexture; }
	eTextureType GetTextureType() const { return textureType; }
	void SetTextureType(eTextureType aTextureType)
	{
		RawTexture->isCubeMap = (aTextureType == eTextureType::CubeMap);
		textureType = aTextureType;
	}


	void InspectorView() override;
private:
#if WorkingOnPngLoading
	bool LoadPngTexture(Texture* outTexture,const std::filesystem::path& aFileName);
#endif
	eTextureType textureType;
	std::shared_ptr<Texture> RawTexture;
};

REFL_AUTO(type(TextureHolder))