#include "AssetManager.pch.h"

#include <Engine/GraphicsEngine/Shaders/Registers.h>

#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include "Engine/GraphicsEngine/GraphicsEngine.h"

#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"

bool Material::CreateJson(const DataMaterial& data, const std::filesystem::path& writePath)
{
	auto json = nlohmann::json::basic_json();
	{
		nlohmann::json& js = json["MaterialBuffer"];
		js["albedoColor"][0] = data.materialData.albedoColor[0];
		js["albedoColor"][1] = data.materialData.albedoColor[1];
		js["albedoColor"][2] = data.materialData.albedoColor[2];
		js["albedoColor"][3] = data.materialData.albedoColor[3];

		js["UVTiling"][0] = data.materialData.UVTiling[0];
		js["UVTiling"][1] = data.materialData.UVTiling[1];

		js["NormalStrength"] = data.materialData.NormalStrength;
		js["Shine"] = data.materialData.Shine;
		js["Roughness"] = data.materialData.Roughness;
	}

	{
		int i = 0;
		for (auto& [path, ptr] : data.textures)
		{
			nlohmann::json arr;
			arr["TextureName"] = path.filename();
			arr["TexturePath"] = path;
			arr["TextureType"] = i;
			i++;

			json["Textures"].push_back(arr);
		}
	}

	std::ofstream stream(ENGINE_RESOURCES.Directory() / writePath.string());
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
	data.textures[static_cast<int>(eTextureType::ColorMap)].second =
		GraphicsEngineInstance.GetDefaultTexture(eTextureType::ColorMap);
	data.textures[static_cast<int>(eTextureType::NormalMap)].second =
		GraphicsEngineInstance.GetDefaultTexture(eTextureType::NormalMap);
	data.textures[static_cast<int>(eTextureType::MaterialMap)].second =
		GraphicsEngineInstance.GetDefaultTexture(eTextureType::MaterialMap);
	data.textures[static_cast<int>(eTextureType::EffectMap)].second =
		GraphicsEngineInstance.GetDefaultTexture(eTextureType::EffectMap);

	if (GraphicsEngineInstance.GetDefaultMaterial())
	{
		data.materialData =
			GraphicsEngineInstance.GetDefaultMaterial()
				->data.materialData; // yo dawg i put some data in your data so you can data while you data
	}
	else
	{
		data.materialData = MaterialBuffer();
	}

	data.vertexShader = GraphicsEngineInstance.GetDefaultVSShader();
	data.pixelShader = GraphicsEngineInstance.GetDefaultPSShader();

	if (exists(AssetPath) && AssetPath.extension() == ".json")
	{
		nlohmann::json json;
		try
		{
			std::ifstream file(AssetPath);
			assert(file.is_open());

			json = nlohmann::json::parse(file);
			file.close();

			nlohmann::json& js = json["MaterialBuffer"];

			data.materialData.albedoColor[0] = js["albedoColor"][0];
			data.materialData.albedoColor[1] = js["albedoColor"][1];
			data.materialData.albedoColor[2] = js["albedoColor"][2];
			data.materialData.albedoColor[3] = js["albedoColor"][3];

			data.materialData.UVTiling[0] = js["UVTiling"][0];
			data.materialData.UVTiling[1] = js["UVTiling"][1];

			data.materialData.NormalStrength = js["NormalStrength"];
			data.materialData.Roughness = js["Roughness"];
			data.materialData.Shine = js["Shine"];
		}
		catch (const std::exception& e)
		{
			const std::string msg =
				"Unsuccessfull loading of material data file at path: " + AssetPath.string() + " " + e.what();
			LOGGER.Warn(msg);
			isLoadedComplete = false;
			return;
		}

		try
		{
			nlohmann::json& js = json["Textures"];
			for (const auto& i : js)
			{
				std::shared_ptr<TextureHolder> texture;
				const std::filesystem::path    path = i["TexturePath"];

				if (path.empty())
				{
					continue;
				}

				texture = ENGINE_RESOURCES.LoadAsset<TextureHolder>(path);

				const int type = i["TextureType"];
				texture->SetTextureType(static_cast<eTextureType>(type));

				data.textures[type].first = path;
				data.textures[type].second = texture;
			}
		}
		catch (const std::exception& e)
		{
			const std::string msg =
				"Unsuccessfull loading of material texture file at path: " + AssetPath.string() + " " + e.what();
			LOGGER.Warn(msg);
			isLoadedComplete = false;
			return;
		}
	}

	isLoadedComplete = true;
}

bool Material::InspectorView()
{
	if (!AssetBase::InspectorView())
	{
		return false;
	}
	Reflect<Material>();

	// if (ImGui::TreeNodeEx(AssetPath.filename().string().c_str()))
	{
		ImGui::Text("Shader data");

		SwitchableAsset<ShipyardShader>(data.vertexShader, "ContentAsset_ShipyardShader", true);
		SwitchableAsset<ShipyardShader>(data.pixelShader, "ContentAsset_ShipyardShader", true);

		// data.vertexShader.lock()->InspectorView();
		// data.pixelShader.lock()->InspectorView();

		ImGui::Text("Material Data");
		if (ReflectSingleValue(data.m_color, *this, "Material Color"))
		{
			data.materialData.albedoColor = data.m_color.GetRGBA();
		}

		ReflectSingleValue(data.materialData.UVTiling, *this, "UV scaling");

		ReflectSingleValue(data.materialData.NormalStrength, *this, "Normal Strength");

		ReflectSingleValue(data.materialData.Shine, *this, "Shine Strength");

		ReflectSingleValue(data.materialData.Roughness, *this, "Roughness Strength");
		ImGui::Separator();
		ImGui::Spacing();
		for (const auto& [index, pair] : data.textures | std::ranges::views::enumerate)
		{
			if (auto& [path, texture] = pair; texture)
			{
				ImGui::TextCentered(std::string(magic_enum::enum_name(static_cast<eTextureType>(index))));
				ImGui::Separator();
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 128);
				SwitchableAsset<TextureHolder>(texture, "ContentAsset_TextureHolder", true);
				// ImGui::ImageButton(std::format("##{}", AssetPath.string()).c_str(), *RawTexture, {128, 128});
				ImGui::NextColumn();

				ImGui::Text(texture->AssetPath.filename().string().c_str());

				Vector2ui                res;
				uint32_t                 mip;
				DXGI_FORMAT              format{};
				D3D12_RESOURCE_DIMENSION dim{};
				D3D12_RESOURCE_FLAGS     flags{};
				if (const auto& raw = texture->GetRawTexture())
				{
					res = raw->GetResolution();
					format = raw->m_Format;
					if (raw->m_Resource)
					{
						format = raw->m_Resource->GetDesc().Format;
						mip = raw->m_Resource->GetDesc().MipLevels;
						dim = raw->m_Resource->GetDesc().Dimension;
						flags = raw->m_Resource->GetDesc().Flags;
					}
				}
				ImGui::Text(std::format("Resolution: {}x{}", res.x, res.y).c_str());
				ImGui::Text(std::format("Format: {}", magic_enum::enum_name(format)).c_str());
				ImGui::Text(std::format("Flags: {}", magic_enum::enum_name(flags)).c_str());
				ImGui::Text(std::format("Dimension: {}", magic_enum::enum_name(dim)).c_str());
				ImGui::Text(std::format("Mips: {}", mip).c_str());
				ImGui::EndColumns();
			}
		}
	}
	return true;
}

std::shared_ptr<TextureHolder> Material::GetEditorIcon()
{
	auto imageTexture =
		ENGINE_RESOURCES.LoadAsset<TextureHolder>(std::format("INTERNAL_IMAGE_UI_{}", AssetPath.filename().string()));
	if (!imageTexture || !imageTexture->isLoadedComplete)
	{
		const std::shared_ptr<Mesh>     mesh = ENGINE_RESOURCES.LoadAsset<Mesh>("Materials/MaterialPreviewMesh.fbx");
		const std::shared_ptr<Material> materialPreview = ENGINE_RESOURCES.LoadAsset<Material>(AssetPath, true);

		const bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
		const bool materialReady = materialPreview->isLoadedComplete && !materialPreview->isBeingLoaded;

		if (!imageTexture->isBeingLoaded && meshReady && materialReady)
		{
			GraphicsEngineUtilities::GenerateSceneForIcon(mesh, imageTexture, materialPreview);
			LOGGER.Log("Material Preview Queued up");
		}
		else
		{
			imageTexture = ENGINE_RESOURCES.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
		}
	}
	return imageTexture;
}

MaterialBuffer& Material::GetMaterialData()
{
	return data.materialData;
}

std::shared_ptr<TextureHolder> Material::GetTexture(eTextureType type)
{
	OPTICK_EVENT();
	if (data.textures.empty())
	{
		return nullptr;
	}

	if (data.textures.size() >= static_cast<int>(type))
	{
		if (data.textures[static_cast<int>(type)].second)
		{
			return data.textures[static_cast<int>(type)].second;
		}
	}
	return nullptr;
}

void Material::SetColor(const Color& aColor)
{
	data.m_color = aColor;
	data.materialData.albedoColor = data.m_color.GetRGBA();
}

void Material::SetColor(const Vector4f& aColor)
{
	data.m_color = aColor;
	data.materialData.albedoColor = data.m_color.GetRGBA();
}

void Material::SetShader(const std::shared_ptr<ShipyardShader>& aVertexShader,
                         const std::shared_ptr<ShipyardShader>& aPixelShader)
{
	data.vertexShader = aVertexShader;
	data.pixelShader = aPixelShader;
}
