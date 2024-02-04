#pragma once
#include <Engine/GraphicsEngine/Rendering/Texture.h>
#include "BaseAsset.h"

enum class eTextureType
{
	ColorMap = 0,
	NormalMap = 1,
	MaterialMap = 2,
	EffectMap = 3,
	ParticleMap = 4,
	CubeMap = 100
};

class TextureHolder : public AssetBase
{
	friend class Material;
public:
	void Init() override;
	TextureHolder(const std::filesystem::path& aFilePath);
	TextureHolder(const std::filesystem::path& aFilePath,eTextureType atextureType);
	std::shared_ptr<Texture> GetRawTexture() const { return RawTexture; }
	eTextureType GetTextureType() const { return textureType; }
	void SetTextureType(eTextureType aTextureType) { textureType = aTextureType; }
private:
#if WorkingOnPngLoading
	bool LoadPngTexture(Texture* outTexture,const std::filesystem::path& aFileName);
#endif
	eTextureType textureType;
	std::shared_ptr<Texture> RawTexture;
};