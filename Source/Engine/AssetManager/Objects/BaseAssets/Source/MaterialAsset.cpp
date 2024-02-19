#include "AssetManager.pch.h" 
#include <Engine/GraphicsEngine/Shaders/Registers.h> 

#include "Engine/GraphicsEngine/GraphicsEngine.h"

bool Material::CreateJson(const DataMaterial& data,const std::filesystem::path& writePath)
{
	nlohmann::json json = nlohmann::json::basic_json();
	{
		nlohmann::json& js = json["MaterialData"];
		js["albedoColor"][0] = data.materialData.Data.albedoColor[0];
		js["albedoColor"][1] = data.materialData.Data.albedoColor[1];
		js["albedoColor"][2] = data.materialData.Data.albedoColor[2];
		js["albedoColor"][3] = data.materialData.Data.albedoColor[3];

		js["UVTiling"][0] = data.materialData.Data.UVTiling[0];
		js["UVTiling"][1] = data.materialData.Data.UVTiling[1];

		js["NormalStrength"] = data.materialData.Data.NormalStrength;
		js["Shine"] = data.materialData.Data.Shine;
	}

	{
		int i = 0;
		for (auto& [path,ptr] : data.textures)
		{
			nlohmann::json arr;
			arr["TextureName"] = path.filename();
			arr["TexturePath"] = path;
			arr["TextureType"] = i;
			i++;

			json["Textures"].push_back(arr);
		}
	}

	std::ofstream stream(AssetManager::Get().AssetPath / writePath.string());
	stream << std::setw(4) << json;
	stream.close();
	return true;
}

Material::Material(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{

}

void Material::Init()
{
	data.textures.resize(4);
	data.textures[(int)eTextureType::ColorMap].second = GraphicsEngine::Get().GetDefaultTexture(eTextureType::ColorMap);
	data.textures[(int)eTextureType::NormalMap].second = GraphicsEngine::Get().GetDefaultTexture(eTextureType::NormalMap);
	data.textures[(int)eTextureType::MaterialMap].second = GraphicsEngine::Get().GetDefaultTexture(eTextureType::MaterialMap);
	data.textures[(int)eTextureType::EffectMap].second = GraphicsEngine::Get().GetDefaultTexture(eTextureType::EffectMap);

	if (GraphicsEngine::Get().GetDefaultMaterial() != nullptr)
	{
		data.materialData.Data = GraphicsEngine::Get().GetDefaultMaterial()->data.materialData.Data; //yo dawg i put some data in your data so you can data while you data
	}
	else
	{
		data.materialData.Data = MaterialData();
	}
	data.vertexShader = GraphicsEngine::Get().GetDefaultVSShader();
	data.pixelShader = GraphicsEngine::Get().GetDefaultPSShader();

	//std::shared_ptr<TextureHolder> text = AssetManager::GetInstance().LoadAsset<TextureHolder>("",true);
	//textures[(int)text->textureType] = text;

	if (std::filesystem::exists(AssetPath) && AssetPath.extension() == ".json")
	{
		std::ifstream file(AssetPath);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();
		{
			try
			{
				nlohmann::json& js = json["MaterialData"];

				data.materialData.Data.albedoColor[0] = js["albedoColor"][0];
				data.materialData.Data.albedoColor[1] = js["albedoColor"][1];
				data.materialData.Data.albedoColor[2] = js["albedoColor"][2];
				data.materialData.Data.albedoColor[3] = js["albedoColor"][3];

				data.materialData.Data.UVTiling[0] = js["UVTiling"][0];
				data.materialData.Data.UVTiling[1] = js["UVTiling"][1];

				data.materialData.Data.NormalStrength = js["NormalStrength"];
				data.materialData.Data.Shine = js["Shine"];
			}
			catch (const std::exception& e)
			{
				std::cout << "Unsuccessfull loading of material data file at path: " << AssetPath << " " << e.what() << "\n";
			}
		}
		{
			try
			{
				nlohmann::json& js = json["Textures"];
				for (const auto& i : js)
				{
					std::shared_ptr<TextureHolder> texture;
					const std::filesystem::path path = i["TexturePath"];
					AssetManager::Get().LoadAsset<TextureHolder>(path,texture);
					data.textures[(int)i["TextureType"]].first = path;
					data.textures[(int)i["TextureType"]].second = texture;
				}
			}
			catch (const std::exception&)
			{
				std::cout << "Unsuccessfull loading of material texture files at path: " << AssetPath << "\n";
			}
		}
	}
	else
	{
		isLoadedComplete = false;
	}

	data.materialData.Initialize();
	isLoadedComplete = true;
}

MaterialData& Material::GetMaterialData()
{
	return data.materialData.Data;
}

void Material::Update()
{
	OPTICK_EVENT();
	if (!isLoadedComplete)
	{
		GraphicsEngine::Get().GetDefaultMaterial()->Update();
		return;
		//REFACTOR
		//If default material is not loaded with forced or if it erronous we will end with a overflow here, guess it guarantees defaults works atleast 
	}

	/*RHI::UpdateConstantBufferData(data.materialData);
	RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_DefaultMaterialBuffer,data.materialData);*/
	SetAsResources();
}

void Material::SetShader(const std::shared_ptr<Shader>& aVertexShader,const std::shared_ptr<Shader>& aPixelShader)
{
	data.vertexShader = aVertexShader;
	data.pixelShader = aPixelShader;
}

void Material::SetAsResources()
{
	/*RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)eTextureType::ColorMap,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)eTextureType::NormalMap,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)eTextureType::MaterialMap,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)eTextureType::EffectMap,nullptr);
	for (const auto& [path,i] : data.textures)
	{
		if (!i)
		{
			continue;
		}

		if (i->isLoadedComplete)
		{
			RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)i->textureType,i->GetRawTexture().get());
		}
		else
		{
			RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,(int)i->textureType,GraphicsEngine::Get().GetDefaultTexture(i->textureType)->GetRawTexture().get());
		}
	}*/
}