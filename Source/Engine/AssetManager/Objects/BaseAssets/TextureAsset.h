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
	void SetRawTexture(const std::shared_ptr<Texture>& aTexture) { RawTexture = aTexture; }
	void SetRawTexture(  Texture  aTexture) { *RawTexture = aTexture; }
	eTextureType GetTextureType() const { return textureType; }
	void SetTextureType(eTextureType aTextureType);
	void InspectorView() override;
private: 
	eTextureType textureType;
	std::shared_ptr<Texture> RawTexture;
};

REFL_AUTO(type(TextureHolder))