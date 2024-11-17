#include "../MeshAsset.h"
#include "AssetManager.pch.h"
#include <Tools/Utilities/Math.hpp>

#include "DirectX/DX12/Graphics/GPU.h"
#include "DirectX/DX12/Graphics/Helpers.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include <Engine/PersistentSystems/SceneUtilities.h>
#include <Tools/ImGui/ImGuiHelpers.hpp>

Mesh::Mesh(const std::filesystem::path &aFilePath) : AssetBase(aFilePath)
{
}
const std::unordered_map<unsigned int, std::shared_ptr<Material>> &Mesh::GetMaterialList()
{
    return materials;
}

void Mesh::FillMaterialPaths(const aiScene *scene)
{
    OPTICK_EVENT();
    for (auto &[key, matPath] : idToMaterial)
    {
        std::filesystem::path texturePath;
        int textureLoaded = 0;
        Vector4f color;
        auto material = scene->mMaterials[key];

        Material::DataMaterial dataMat;
        dataMat.textures.resize(4);

        aiString str;
        matPath = "Materials/Temporary/";
        matPath += material->GetName().C_Str();

        std::wstring mdf = matPath.wstring();
        std::wstring illegalChars = L":?\"<>|";
        for (auto &i : mdf)
        {
            bool found = illegalChars.find(i) != std::string::npos;
            if (found)
            {
                i = '_';
            }
        }
        matPath = mdf;
        matPath.replace_extension("json");

        // for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
        //{
        //	auto type = static_cast<aiTextureType>(i);
        //	std::string textureAmount = std::string(aiTextureTypeToString(type)) + " " +
        // std::to_string(material->GetTextureCount(type)); 	if (material->GetTextureCount(type))
        //	{
        //		Logger.Log(textureAmount);
        //	}
        // }

        std::pair<std::filesystem::path, std::shared_ptr<TextureHolder>> holder;
        if (material->GetTextureCount(aiTextureType_BASE_COLOR))
        {
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[0] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_DIFFUSE) && !str.length)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[0] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_HEIGHT))
        {
            material->GetTexture(aiTextureType_HEIGHT, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[1] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_NORMALS))
        {
            material->GetTexture(aiTextureType_NORMALS, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[1] = holder;
            textureLoaded++;
        }
        if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
        {
            material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[2] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR))
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[2] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
        {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[2] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_METALNESS))
        {
            material->GetTexture(aiTextureType_METALNESS, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[2] = holder;
            textureLoaded++;
        }

        if (material->GetTextureCount(aiTextureType_EMISSIVE))
        {
            material->GetTexture(aiTextureType_EMISSIVE, 0, &str);
            holder.first = str.C_Str();
            dataMat.textures[3] = holder;
            textureLoaded++;
        }

        if (!textureLoaded)
        {
            materials[key] = GraphicsEngineInstance.GetDefaultMaterial();
            continue;
        }

        Material::CreateJson(dataMat, matPath);
        materials[key] = EngineResources.LoadAsset<Material>(matPath);
    }
}

std::shared_ptr<TextureHolder> Mesh::GetEditorIcon()
{
	OPTICK_EVENT();
    auto imageTexture =
        EngineResources.LoadAsset<TextureHolder>(std::format("INTERNAL_IMAGE_UI_{}", AssetPath.filename().string()));
    std::shared_ptr<Mesh> mesh = EngineResources.LoadAsset<Mesh>(AssetPath, true);

    if (!imageTexture->isLoadedComplete)
    {
        const bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
        if (!imageTexture->isBeingLoaded && meshReady)
        {
            GraphicsEngineUtilities::GenerateSceneForIcon(mesh, imageTexture,
                                                          GraphicsEngineInstance.GetDefaultMaterial());
        }
        else
        {
            imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
        }
    }
    return imageTexture;
}

bool Mesh::InspectorView()
{
	OPTICK_EVENT();
    if (!AssetBase::InspectorView())
    {
        return false;
    }

    {
        auto imageTexture = EngineResources.LoadAsset<TextureHolder>(
            std::format("INTERNAL_IMAGE_UI_{}", AssetPath.filename().string()));
        std::shared_ptr<Mesh> mesh = EngineResources.LoadAsset<Mesh>(AssetPath, true);

        if (!imageTexture->isLoadedComplete)
        {
            const bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
            if (!imageTexture->isBeingLoaded && meshReady)
            {
                GraphicsEngineUtilities::GenerateSceneForIcon(mesh, imageTexture,
                                                              GraphicsEngineInstance.GetDefaultMaterial());
            }
            else
            {
                imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
            }
        }

        ImGui::BeginChild("MeshChild");
        if (ImGui::BeginTable("Mesh", 2))
        {
            ImGui::TableNextColumn();
            ImGui::Text("Static Mesh");
            ImGui::TableNextColumn();
            ImGui::Image(imageTexture, {100, 100});
            ImGui::EndTable();
        }
        ImGui::Separator();
    }

    ImGui::PushID(this);
    if (ImGui::TreeNodeEx("Integrated materials")) // Replace with element name
    {
        ImGui::Separator();
        for (const auto &element : Elements)
        {
            if (materials.contains(element.MaterialIndex))
            {
                materials.at(element.MaterialIndex)->InspectorView();
            }
            ImGui::Separator();
        }

        ImGui::TreePop();
    }
    ImGui::PopID();

    ImGui::EndChild();
    return true;
}

void Mesh::Init()
{
    OPTICK_EVENT();
    if (!std::filesystem::exists(AssetPath))
    {
        Logger.Warn("Failed to load mesh at: " + AssetPath.string());
        return;
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        AssetPath.string(), (unsigned int)aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                aiProcess_ValidateDataStructure | aiProcess_SortByPType | aiProcess_GenBoundingBoxes |
                                aiProcess_GlobalScale | aiProcessPreset_TargetRealtime_MaxQuality |
                                aiProcess_ConvertToLeftHanded);

    auto exception = importer.GetErrorString();
    if (importer.GetException())
    {
        Logger.Err(exception);
        return;
    }

    if (scene->HasAnimations())
    {
        Logger.Warn("Can not load animations as mesh: " + AssetPath.string());
        return;
    }

    // If the import failed, report it
    if (nullptr == scene)
    {
        std::string message = "Failed to load mesh " + AssetPath.string();
        std::exception failedMeshLoad(message.c_str());
        Logger.Critical(failedMeshLoad, 2);
        return;
    }

    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
		Element element;
		bool succeded = processMesh(scene->mMeshes[i], scene, element);

        if (succeded)
        {
		    idToMaterial.try_emplace(element.MaterialIndex, "");
            Bounds.Extend(element.Bounds);
		    Elements.emplace_back(element);
        }
    }

    // FillMatPath
    FillMaterialPaths(scene);
    isLoadedComplete = true;
    return;
}

bool Mesh::processMesh(aiMesh *mesh, const aiScene *scene, Element& outElement)
{
    scene;
    OPTICK_EVENT(); 

    std::vector<Vertex> mdlVertices;
    mdlVertices.reserve(mesh->mNumVertices);
    std::vector<uint32_t> mdlIndicies;
    mdlIndicies.reserve(mesh->mNumFaces * 3);

    std::string name = mesh->mName.C_Str();
    std::wstring wname = Helpers::string_cast<std::wstring>(name).c_str();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        auto position = Vector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        auto color = Vector4f(RandomEngine::RandomInRange<float>(0, 1), RandomEngine::RandomInRange<float>(0, 1),
                              RandomEngine::RandomInRange<float>(0, 1), 1.0f);

        auto boneId = Vector4<unsigned int>(0, 0, 0, 0);

        /*auto boneWeight = Vector4f(
            vert.BoneWeights[0],
            vert.BoneWeights[1],
            vert.BoneWeights[2],
            vert.BoneWeights[3],
        );*/

        auto boneWeight = Vector4f(0, 0, 0, 0);

        auto UVCoord = Vector2f();
        if (mesh->HasTextureCoords(0))
        {
            UVCoord = Vector2f(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }

        auto normal = Vector3f();
        if (mesh->HasNormals())
        {
            normal = Vector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        auto tangent = Vector3f();
        if (mesh->HasTangentsAndBitangents())
        {
            tangent = Vector3f(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        }
        mdlVertices.emplace_back(position, color, UVCoord, normal, tangent, boneId, boneWeight);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            mdlIndicies.push_back(face.mIndices[j]);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            const auto &vertBoneData = mesh->mBones[i]->mWeights[j];
            mdlVertices[vertBoneData.mVertexId].AddBoneWeightAndID(vertBoneData.mWeight, i);
        }
    }

    auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = commandQueue->GetCommandList();

    OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());

    VertexResource vertexRes(wname);
    if (!GPUInstance.CreateVertexBuffer<Vertex>(commandList, vertexRes, mdlVertices))
    {
        Logger.Err("Failed to create vertex buffer");
        return false;
    }

    IndexResource indexRes(wname);
    if (!GPUInstance.CreateIndexBuffer(commandList, indexRes, mdlIndicies))
    {
        Logger.Err("Failed to create index buffer");
        return false;
    }

    auto fenceValue = commandQueue->ExecuteCommandList(commandList);
    commandQueue->WaitForFenceValue(fenceValue);
    commandQueue->Flush();

    outElement.VertexBuffer = vertexRes;
    outElement.IndexResource = indexRes;
    outElement.Stride = sizeof(Vertex);
    outElement.MaterialIndex = mesh->mMaterialIndex;
     
	outElement.Bounds = {
		Vector3f(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z),
		Vector3f(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z)
    };

    return true;
}

void Mesh::ResizeBuffer()
{
    // D3D11_SUBRESOURCE_DATA vertexSubResourceData{};
    // vertexSubResourceData.pSysMem = myInstances.data();
    // vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Matrix) * myInstances.size());
    // HRESULT result;
    // result = RHI::Device->CreateBuffer(
    //	&vertexBufferDesc,
    //	&vertexSubResourceData,
    //	myInstanceBuffer.GetAddressOf()
    //);
    //
    // if (FAILED(result))
    //{
    //	Logger.Log("Failed to create Instance buffer");
    //	return;
    // }
    // bufferSize = static_cast<int>(myInstances.size());
}

void Mesh::UpdateInstanceBuffer()
{
    OPTICK_EVENT();
    // if (myInstances.size() > bufferSize)
    //{
    //	ResizeBuffer();
    // }
    // D3D11_MAPPED_SUBRESOURCE mappedResource{};
    // RHI::Context->Map(myInstanceBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedResource);
    // memcpy(mappedResource.pData,myInstances.data(),sizeof(Matrix) * myInstances.size());
    // RHI::Context->Unmap(myInstanceBuffer.Get(),0);
}
