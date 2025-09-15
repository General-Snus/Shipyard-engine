#pragma once
#include "BaseAsset.h"
#include "DirectX/DX12/Graphics/Resources/Texture.h"
#include "Engine/AssetManager/Reflection/Reflectable.h"
#include "Engine/AssetManager/Interfaces.h"
#include "Engine/AssetManager/Enums.h"

class Texture;

class TextureHolder : public AssetBase, public Reflectable<TextureHolder>
{
	friend class Material;

public:
	reflectable(TextureHolder);
	void Init() override;
	TextureHolder(const std::filesystem::path& aFilePath);
	TextureHolder(const std::filesystem::path& aFilePath, eTextureType atextureType);

	std::shared_ptr<Texture> GetRawTexture() const
	{
		return RawTexture;
	}

	void SetRawTexture(const std::shared_ptr<Texture>& aTexture)
	{
		RawTexture = aTexture;
	}

	void SetRawTexture(Texture aTexture)
	{
		*RawTexture = aTexture;
	}

	eTextureType GetTextureType() const
	{
		return textureType;
	}
	void SetTextureType(eTextureType aTextureType);
	bool InspectorView() override;

	std::shared_ptr<TextureHolder> GetEditorIcon() override;

private:
	eTextureType textureType;
	std::shared_ptr<Texture> RawTexture;
};

REFL_AUTO(type(TextureHolder))
