#include "AssetManager.pch.h"
#include <GraphicsEngine/GraphicsEngine.pch.h>
#include "MaterialAsset.h"
#include <ThirdParty/nlohmann/json.hpp>
#include <fstream>
#include <GraphicsEngine/Shaders/Registers.h>

Material::Material(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	
}

void Material::Init()
{
	textures.resize(4);
	textures[(int)eTextureType::ColorMap] = GraphicsEngine::Get().GetDefaultTexture(eTextureType::ColorMap);
	textures[(int)eTextureType::NormalMap] = GraphicsEngine::Get().GetDefaultTexture(eTextureType::NormalMap);
	textures[(int)eTextureType::MaterialMap] = GraphicsEngine::Get().GetDefaultTexture(eTextureType::MaterialMap);
	textures[(int)eTextureType::EffectMap] = GraphicsEngine::Get().GetDefaultTexture(eTextureType::EffectMap);

	if(GraphicsEngine::Get().GetDefaultMaterial() != nullptr)
	{
		materialData.Data = GraphicsEngine::Get().GetDefaultMaterial()->materialData.Data;
	}
	else
	{
		materialData.Data = MaterialData();
	}
	vertexShader = GraphicsEngine::Get().GetDefaultVSShader();
	pixelShader = GraphicsEngine::Get().GetDefaultPSShader();

	//std::shared_ptr<TextureHolder> text = AssetManager::GetInstance().LoadAsset<TextureHolder>("",true);
	//textures[(int)text->textureType] = text;

	if(std::filesystem::exists(AssetPath) && AssetPath.extension() == ".json")
	{
		std::ifstream file(AssetPath);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();
		{
			try
			{
				nlohmann::json& data = json["MaterialData"];

				materialData.Data.albedoColor[0] = data["albedoColor"][0];
				materialData.Data.albedoColor[1] = data["albedoColor"][1];
				materialData.Data.albedoColor[2] = data["albedoColor"][2];
				materialData.Data.albedoColor[3] = data["albedoColor"][3];

				materialData.Data.UVTiling[0] = data["UVTiling"][0];
				materialData.Data.UVTiling[1] = data["UVTiling"][1];

				materialData.Data.NormalStrength = data["NormalStrength"];
				materialData.Data.Shine = data["Shine"];
			}
			catch(const std::exception&)
			{
				std::cout << "Unsuccessfull loading of material data file at path: " << AssetPath << "\n";
			}
		}
		{
			try
			{
				nlohmann::json& data = json["Textures"];
				for(auto& i : data)
				{
					std::shared_ptr<TextureHolder> texture;
					AssetManager::GetInstance().LoadAsset<TextureHolder>(i["TexturePath"],texture);
					textures[(int)i["TextureType"]] = texture;
				}
			}
			catch(const std::exception&)
			{
				std::cout << "Unsuccessfull loading of material texture files at path: " << AssetPath << "\n";
			}
		}
	}

	materialData.Initialize();
	isLoadedComplete = true;
}
//void Material::AddTexture(const std::filesystem::path& aFilePath)
//{
//	std::shared_ptr<TextureHolder> text = AssetManager::GetInstance().LoadAsset<TextureHolder>(aFilePath);
//	if(!text)
//	{
//		return;
//	}
//	textures[(int)text->textureType] = text;
//}
//void Material::AddTexture(const std::shared_ptr<TextureHolder> text)
//{
//	if(!text)
//	{
//		return;
//	}
//	textures[(int)text->textureType] = text;
//}
MaterialData& Material::GetMaterialData()
{
	return materialData.Data;
}
void Material::Update()
{
	if(!isLoadedComplete)
	{
		GraphicsEngine::Get().GetDefaultMaterial()->Update();
		return;
		//REFACTOR
		//If default material is not loaded with forced or if it erronous we will end with a overflow here, guess it guarantees defaults works atleast 
	}

	//ComPtr<ID3D11VertexShader> convertedVS;
	//vertexShader.lock()->GetShader().As(&convertedVS);
	//RHI::SetVertexShader(convertedVS); 

	//ComPtr<ID3D11PixelShader> convertedPS;
	//pixelShader.lock()->GetShader().As(&convertedPS);
	//RHI::SetPixelShader(convertedPS);

	RHI::UpdateConstantBufferData(materialData);
	RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_DefaultMaterialBuffer,materialData);

	 SetAsResources();
}

void Material::SetShader(std::shared_ptr<Shader> aVertexShader,std::shared_ptr<Shader> aPixelShader)
{ 
	vertexShader = aVertexShader;
	pixelShader = aPixelShader;
}

void Material::SetAsResources()
{
	for(auto& i : textures)
	{
		if(!i)
		{
			continue;
		}

		if(i->isLoadedComplete)
		{
			RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)i->textureType,i->GetRawTexture().get());
		}
		else
		{
			RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)i->textureType,GraphicsEngine::Get().GetDefaultTexture(i->textureType)->GetRawTexture().get());
		}
	}
}