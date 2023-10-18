#pragma once 
#include "BaseAsset.h"
#include <Engine/GraphicsEngine/Objects/Shader.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Thirdparty/nlohmann/json.hpp>

class TextureHolder;

struct MaterialData
{
	CU::Vector4<float> albedoColor = {1.f,1.f,1.f,1.f};
	CU::Vector2<float> UVTiling = {1.f,1.f};
	float NormalStrength = 1.0f;
	float Shine = 100.f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MaterialData,
	albedoColor.x,albedoColor.y,albedoColor.z,albedoColor.w,
	UVTiling.x,UVTiling.y,NormalStrength,Shine
);

class Material : public AssetBase
{
public:
	Material(const std::filesystem::path& aFilePath); // Json path
	void Init() override;

	//void AddTexture(const std::filesystem::path& aFilePath);
	//void AddTexture(const std::shared_ptr<TextureHolder> text);
	MaterialData& GetMaterialData();
	void Update();
	void SetShader(std::shared_ptr<Shader> aVertexShader,std::shared_ptr<Shader> aPixelShader);
	void SetAsResources();
private:
	std::weak_ptr<Shader> vertexShader;
	std::weak_ptr<Shader> pixelShader;
	ConstantBuffer<MaterialData> materialData;
	std::vector <std::shared_ptr<TextureHolder>> textures;
};
