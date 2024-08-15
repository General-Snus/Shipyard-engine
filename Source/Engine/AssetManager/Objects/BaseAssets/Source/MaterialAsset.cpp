#include "Engine/AssetManager/AssetManager.pch.h"

#include <Engine/GraphicsEngine/Shaders/Registers.h>

#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>

#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>

bool Material::CreateJson(const DataMaterial &data, const std::filesystem::path &writePath)
{
    nlohmann::json json = nlohmann::json::basic_json();
    {
        nlohmann::json &js = json["MaterialBuffer"];
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
        for (auto &[path, ptr] : data.textures)
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

Material::Material(const std::filesystem::path &aFilePath) : AssetBase(aFilePath)
{
}

void Material::Init()
{
    data.textures.resize(4);
    data.textures[static_cast<int>(eTextureType::ColorMap)].second =
        GraphicsEngine::Get().GetDefaultTexture(eTextureType::ColorMap);
    data.textures[static_cast<int>(eTextureType::NormalMap)].second =
        GraphicsEngine::Get().GetDefaultTexture(eTextureType::NormalMap);
    data.textures[static_cast<int>(eTextureType::MaterialMap)].second =
        GraphicsEngine::Get().GetDefaultTexture(eTextureType::MaterialMap);
    data.textures[static_cast<int>(eTextureType::EffectMap)].second =
        GraphicsEngine::Get().GetDefaultTexture(eTextureType::EffectMap);

    if (GraphicsEngine::Get().GetDefaultMaterial() != nullptr)
    {
        data.materialData =
            GraphicsEngine::Get()
                .GetDefaultMaterial()
                ->data.materialData; // yo dawg i put some data in your data so you can data while you data
    }
    else
    {
        data.materialData = MaterialBuffer();
    }

    data.vertexShader = GraphicsEngine::Get().GetDefaultVSShader();
    data.pixelShader = GraphicsEngine::Get().GetDefaultPSShader();

    if (std::filesystem::exists(AssetPath) && AssetPath.extension() == ".json")
    {
        nlohmann::json json;
        try
        {
            std::ifstream file(AssetPath);
            assert(file.is_open());

            json = nlohmann::json::parse(file);
            file.close();
            isLoadedComplete = true; // will be set to faalse if failing any where, bad but im not gonna be bothered

            nlohmann::json &js = json["MaterialBuffer"];

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
        catch (const std::exception &e)
        {
            std::string msg =
                "Unsuccessfull loading of material data file at path: " + AssetPath.string() + " " + e.what();
            Logger::Warn(msg);
            isLoadedComplete = false;
        }

        try
        {
            nlohmann::json &js = json["Textures"];
            for (const auto &i : js)
            {
                std::shared_ptr<TextureHolder> texture;
                const std::filesystem::path path = i["TexturePath"];

                if (path.empty())
                {
                    continue;
                }

                texture = AssetManager::Get().LoadAsset<TextureHolder>(path);

                const int type = i["TextureType"];
                texture->SetTextureType(static_cast<eTextureType>(type));

                data.textures[type].first = path;
                data.textures[type].second = texture;
            }
        }
        catch (const std::exception &e)
        {
            std::string msg =
                "Unsuccessfull loading of material texture file at path: " + AssetPath.string() + " " + e.what();
            Logger::Warn(msg);
            isLoadedComplete = false;
        }
    }
    else
    {
        isLoadedComplete = false;
    }
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
        data.vertexShader.lock()->InspectorView();
        data.pixelShader.lock()->InspectorView();

        ImGui::Text("Material Data");
        Reflectable::UpdateCleanValue(data.m_color, *this, "Material Color");
        data.materialData.albedoColor = data.m_color.GetRGBA();

        Reflectable::UpdateCleanValue(data.materialData.UVTiling, *this, "UV scaling");

        Reflectable::UpdateCleanValue(data.materialData.NormalStrength, *this, "Normal Strength");

        Reflectable::UpdateCleanValue(data.materialData.Shine, *this, "Shine Strength");

        Reflectable::UpdateCleanValue(data.materialData.Roughness, *this, "Roughness Strength");

        for (const auto &[index, pair] : data.textures | std::ranges::views::enumerate)
        {
            auto &[path, texture] = pair;
            if (texture)
            {
                ImGui::TextCentered(std::string(magic_enum::enum_name(eTextureType(index))));
                ImGui::Separator();
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 128);
                SwitchableAsset<TextureHolder>(texture, "ContentAsset_TextureHolder", true);
                // ImGui::ImageButton(std::format("##{}", AssetPath.string()).c_str(), *RawTexture, {128, 128});
                ImGui::NextColumn();

                ImGui::Text(texture->AssetPath.filename().string().c_str());

                Vector2ui res;
                uint32_t mip;
                DXGI_FORMAT format{};
                D3D12_RESOURCE_DIMENSION dim{};
                D3D12_RESOURCE_FLAGS flags{};
                if (texture; const auto &raw = texture->GetRawTexture())
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
    auto imageTexture = AssetManager::Get().LoadAsset<TextureHolder>(
        std::format("INTERNAL_IMAGE_UI_{}", AssetPath.filename().string()));
    if (!imageTexture || !imageTexture->isLoadedComplete)
    {
        std::shared_ptr<Mesh> mesh = AssetManager::Get().LoadAsset<Mesh>("Materials/MaterialPreviewMesh.fbx");
        std::shared_ptr<Material> materialPreview = AssetManager::Get().LoadAsset<Material>(AssetPath, true);

        bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
        bool materialReady = materialPreview->isLoadedComplete && !materialPreview->isBeingLoaded;

        if (!imageTexture->isBeingLoaded && meshReady && materialReady)
        {
            GraphicsEngineUtilities::GenerateSceneForIcon(mesh, imageTexture, materialPreview);
            Logger::Log("Material Preview Queued up");
        }
        else
        {
            imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
        }
    }
    return imageTexture;
}

MaterialBuffer &Material::GetMaterialData()
{
    return data.materialData;
}

void Material::Update()
{
    OPTICK_EVENT();
    if (!isLoadedComplete)
    {
        GraphicsEngine::Get().GetDefaultMaterial()->Update();
        return;
        // REFACTOR
        // If default material is not loaded with forced or if it erronous we will end with a overflow here, guess it
        // guarantees defaults works atleast
    }

    /*RHI::UpdateConstantBufferData(data.materialData);
    RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER |
    PIPELINE_STAGE_PIXEL_SHADER,REG_DefaultMaterialBuffer,data.materialData);*/
    SetAsResources();
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

void Material::SetShader(const std::shared_ptr<ShipyardShader> &aVertexShader,
                         const std::shared_ptr<ShipyardShader> &aPixelShader)
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
