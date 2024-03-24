#pragma once 
#include <Engine/GraphicsEngine/Objects/Shader.h>
#include <Engine/AssetManager/Enums.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Tools/Thirdparty/nlohmann/json.hpp>
#include "BaseAsset.h"

class TextureHolder;

struct MaterialBuffer
{
	Vector4f albedoColor = { 1.f,1.f,1.f,1.f };
	Vector2f UVTiling = { 1.f,1.f };
	float NormalStrength = 1.0f;
	float Shine = 100.f;
	int albedoTexture = -1;
	int normalTexture = -1;
	int materialTexture = -1;
	int emissiveTexture = -1;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MaterialBuffer,
	albedoColor.x,albedoColor.y,albedoColor.z,albedoColor.w,
	UVTiling.x,UVTiling.y,NormalStrength,Shine
); 

class Material : public AssetBase
{
	friend class cMeshRenderer;
public:
	struct DataMaterial
	{
		std::weak_ptr<Shader> vertexShader;
		std::weak_ptr<Shader> pixelShader;
		ConstantBuffer<MaterialBuffer> materialData;
		std::vector<std::pair<std::filesystem::path,std::shared_ptr<TextureHolder>>> textures;
	};
	static bool CreateJson(const DataMaterial& data,const std::filesystem::path& pth);


	Material(const std::filesystem::path& aFilePath); // Json path
	void Init() override;
	//void AddTexture(const std::filesystem::path& aFilePath);
	//void AddTexture(const std::shared_ptr<TextureHolder> text);
	MaterialBuffer& GetMaterialData();
	void Update();
	void SetShader(const std::shared_ptr<Shader>& aVertexShader,const std::shared_ptr<Shader>& aPixelShader);
	void SetAsResources();
private:
	std::shared_ptr<TextureHolder> GetTexture(eTextureType );
	DataMaterial data;
};
