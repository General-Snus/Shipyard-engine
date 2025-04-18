#pragma once
#include "BaseAsset.h"
#include <Engine/AssetManager/Enums.h>  

class TextureHolder;
class ShipyardShader;
struct MaterialBuffer
{
    Vector4f albedoColor = {1.f, 1.f, 1.f, 1.f};

    Vector2f UVTiling = {1.f, 1.f};
    float NormalStrength = 1.0f;
    float Shine = 100.f;

    // why default -1? Because debug layer will flip out and give us warning and thats a good thing
    float Roughness = 1.f;
    int albedoTexture = -1;
    int normalTexture = -1;
    int materialTexture = -1;
    int emissiveTexture = -1;

    unsigned int vertexOffset = 0;
    unsigned int vertexBufferIndex = 0;
    float padding;
};

class Material : public AssetBase
{
    friend class MeshRenderer;

  public:
    ReflectableTypeRegistration();
    struct DataMaterial
    {
        std::shared_ptr<ShipyardShader> vertexShader;
        std::shared_ptr<ShipyardShader> pixelShader;
        Color m_color;
        MaterialBuffer materialData;
        std::vector<std::pair<std::filesystem::path, std::shared_ptr<TextureHolder>>> textures;
    };
    static bool CreateJson(const DataMaterial &data, const std::filesystem::path &pth);
    explicit Material(const std::filesystem::path &aFilePath); // Json path
    void Init() override;
    bool InspectorView() override;

    MaterialBuffer &GetMaterialData();

	void SetColor(const Color &aColor);
	void SetColor(const Vector4f &aColor);
    void SetShader(const std::shared_ptr<ShipyardShader> &aVertexShader,
                   const std::shared_ptr<ShipyardShader> &aPixelShader); 
    std::shared_ptr<TextureHolder> GetEditorIcon() override;

  private:
    std::shared_ptr<TextureHolder> GetTexture(eTextureType);
    DataMaterial data;
};

REFL_AUTO(type(Material))
