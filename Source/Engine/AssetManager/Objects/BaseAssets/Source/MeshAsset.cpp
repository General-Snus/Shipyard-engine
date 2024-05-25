#include "AssetManager.pch.h" 
#include <Tools/Utilities/Math.hpp>
#include "../MeshAsset.h" 

#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/Helpers.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"



Mesh::Mesh(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{

}

const std::unordered_map<unsigned int,std::shared_ptr<Material>>& Mesh::GetMaterialList()
{
	return materials;
}

void Mesh::FillMaterialPaths(const aiScene* scene)
{
	OPTICK_EVENT();
	for (auto& [key,matPath] : idToMaterial)
	{
		std::filesystem::path texturePath;
		int textureLoaded = 0;
		Vector4f color;
		auto material = scene->mMaterials[key];
		material->Get(AI_MATKEY_COLOR_DIFFUSE,color);

		Material::DataMaterial dataMat;
		//dataMat.materialData.Data.albedoColor = color;
		dataMat.textures.resize(4);


		aiString  str;
		matPath = "Materials/Temporary/";
		matPath += material->GetName().C_Str();

		std::wstring mdf = matPath.wstring();
		std::wstring illegalChars = L":?\"<>|";
		for (auto& i : mdf)
		{
			bool found = illegalChars.find(i) != std::string::npos;
			if (found)
			{
				i = '_';
			}
		}
		matPath = mdf;
		matPath.replace_extension("json");

		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			auto type = static_cast<aiTextureType>(i);
			std::string textureAmount = std::string(aiTextureTypeToString(type)) + " " + std::to_string(material->GetTextureCount(type));
			if (material->GetTextureCount(type))
			{
				Logger::Log(textureAmount);
			}
		}


		std::pair<std::filesystem::path,std::shared_ptr<TextureHolder>> holder;
		if (material->GetTextureCount(aiTextureType_BASE_COLOR))
		{
			material->GetTexture(aiTextureType_BASE_COLOR,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[0] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE) && !str.length)
		{
			material->GetTexture(aiTextureType_DIFFUSE,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[0] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT))
		{
			material->GetTexture(aiTextureType_HEIGHT,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[1] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_NORMALS))
		{
			material->GetTexture(aiTextureType_NORMALS,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[1] = holder;
			textureLoaded++;
		}
		if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
		{
			material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[2] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_SPECULAR))
		{
			material->GetTexture(aiTextureType_SPECULAR,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[2] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
		{
			material->GetTexture(aiTextureType_AMBIENT_OCCLUSION,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[2] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_METALNESS))
		{
			material->GetTexture(aiTextureType_METALNESS,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[2] = holder;
			textureLoaded++;
		}

		if (material->GetTextureCount(aiTextureType_EMISSIVE))
		{
			material->GetTexture(aiTextureType_EMISSIVE,0,&str);
			holder.first = str.C_Str();
			dataMat.textures[3] = holder;
			textureLoaded++;
		}

		if (!textureLoaded)
		{
			materials[key] = GraphicsEngine::Get().GetDefaultMaterial();
			continue;
		}
		Material::CreateJson(dataMat,matPath);
		AssetManager::Get().LoadAsset<Material>(matPath,materials[key]);
	}
}

void Mesh::InspectorView()
{
	ImGui::PushID(this);
	AssetBase::InspectorView();
	Reflect<Mesh>();


	if (ImGui::TreeNodeEx("Elements")) // Replace with element name
	{
		ImGui::Separator();
		for (auto element : Elements)
		{
			ImGui::Text("Element");
			ImGui::Text("Vertex count: " + element.Vertices.size());
			ImGui::Text("Index count: " + element.Indicies.size());

			if (materials.contains(element.MaterialIndex))
			{
				materials.at(element.MaterialIndex)->InspectorView();
			}
			ImGui::Separator();
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void Mesh::Init()
{
	OPTICK_EVENT();
	if (!std::filesystem::exists(AssetPath))
	{
		assert(false && "Mesh file does not exist");
	}
#if UseTGAImporter == 1

	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Mesh inMesh;

	if (TGA::FBX::Importer::LoadMeshW(AssetPath,inMesh))
	{
		float scalar = 0.01f;//TODO Scaling
		Vector3f greatestExtent = {
			std::abs(inMesh.BoxSphereBounds.Center[0] - inMesh.BoxSphereBounds.BoxExtents[0]) * scalar,//TODO Scaling
			std::abs(inMesh.BoxSphereBounds.Center[1] - inMesh.BoxSphereBounds.BoxExtents[1]) * scalar,
			std::abs(inMesh.BoxSphereBounds.Center[2] - inMesh.BoxSphereBounds.BoxExtents[2]) * scalar
		};

		float radius = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (radius < greatestExtent[i])
			{
				radius = greatestExtent[i];
			}
		}

		boxSphereBounds = Sphere<float>(
			{
				inMesh.BoxSphereBounds.Center[0] * scalar,
				inMesh.BoxSphereBounds.Center[1] * scalar,//TODO Scaling
				inMesh.BoxSphereBounds.Center[2] * scalar
			},
			radius * scalar
		);

		Editor::GetEditor().ExpandWorldBounds(boxSphereBounds); // TODO Make a scene contain the boxSphereBounds!!
		std::vector<Vertex> mdlVertices;
		std::vector<unsigned int> mdlIndicies;

		for (const auto& element : inMesh.Elements)
		{
			for (const auto& vert : element.Vertices)
			{
				auto position = Vector3f(
					vert.Position[0] * scalar,
					vert.Position[1] * scalar,
					vert.Position[2] * scalar
				);
				auto color = Vector4f(
					RandomInRange<float>(0,1),
					RandomInRange<float>(0,1),
					RandomInRange<float>(0,1),
					1.0f
				);

				auto boneId = Vector4<unsigned int>(
					vert.BoneIDs[0],
					vert.BoneIDs[1],
					vert.BoneIDs[2],
					vert.BoneIDs[3]
				);

				auto boneWeight = Vector4f(
					vert.BoneWeights[0],
					vert.BoneWeights[1],
					vert.BoneWeights[2],
					vert.BoneWeights[3]
				);

				auto UVCoord = Vector2f(
					vert.UVs[0][0],
					vert.UVs[0][1]
				);

				auto normal = Vector3f(
					vert.Normal[0],
					vert.Normal[1],
					vert.Normal[2]
				);

				auto tangent = Vector3f(
					vert.Tangent[0],
					vert.Tangent[1],
					vert.Tangent[2]
				);

				mdlVertices.emplace_back(
					position,
					color,
					UVCoord,
					normal,
					tangent,
					boneId,
					boneWeight
				);
			}
			mdlIndicies = element.Indices;
			ComPtr<ID3D11Buffer> vertexBuffer;
			if (!RHI::CreateVertexBuffer<Vertex>(vertexBuffer,mdlVertices))
			{
				std::cout << "Failed to create vertex buffer" << std::endl;
				return;
			}
			ComPtr<ID3D11Buffer> indexBuffer;
			if (!RHI::CreateIndexBuffer(indexBuffer,mdlIndicies))
			{
				std::cout << "Failed to create vertex buffer" << std::endl;
				return;
			}
			Element toAdd = {
				vertexBuffer,
				indexBuffer,
				AsUINT(mdlVertices.size()),
				AsUINT(mdlIndicies.size()),
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				sizeof(Vertex)
			};

			Elements.push_back(toAdd);
			mdlVertices.clear();
			mdlIndicies.clear();
		}
	}
	MaxBox = Vector3f(inMesh.BoxBounds.Max[0],inMesh.BoxBounds.Max[1],inMesh.BoxBounds.Max[2]);
	MinBox = Vector3f(inMesh.BoxBounds.Min[0],inMesh.BoxBounds.Min[1],inMesh.BoxBounds.Min[2]);
	isLoadedComplete = true;

	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Matrix) * myInstances.size());
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;


	HRESULT result;
	result = RHI::Device->CreateBuffer(
		&vertexBufferDesc,
		nullptr,
		myInstanceBuffer.GetAddressOf()
	);
	bufferSize = 0;
	if (FAILED(result))
	{
		AMLogger.Log("Failed to create Instance buffer");
		return;
}
#else 
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(AssetPath.string(),(unsigned int)
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ValidateDataStructure |
		aiProcess_SortByPType |
		aiProcess_GenBoundingBoxes |
		aiProcess_GlobalScale | aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_ConvertToLeftHanded);

	auto  exception = importer.GetErrorString();
	if (importer.GetException())
	{
		Logger::Err(exception);
		return;
	}
	// If the import failed, report it
	if (nullptr == scene)
	{
		std::string message = "Failed to load mesh " + AssetPath.string();
		std::exception failedMeshLoad(message.c_str());
		Logger::LogException(failedMeshLoad,2);
		return;
	}
	//Editor::Get().ExpandWorldBounds(boxSphereBounds); // TODO Make a scene contain the boxSphereBounds!! 

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		processMesh(scene->mMeshes[i],scene);
	}
	//FillMatPath   
	FillMaterialPaths(scene);

	MaxBox = Vector3f(scene->mMeshes[0]->mAABB.mMax.x,scene->mMeshes[0]->mAABB.mMax.y,scene->mMeshes[0]->mAABB.mMax.z);
	MinBox = Vector3f(scene->mMeshes[0]->mAABB.mMin.x,scene->mMeshes[0]->mAABB.mMin.y,scene->mMeshes[0]->mAABB.mMin.z);
	float radius = (MaxBox - MinBox).Length();

	Vector3f center = Vector3f((MaxBox.x - MinBox.x) / 2,(MaxBox.y - MinBox.y) / 2,(MaxBox.z - MinBox.z) / 2);
	boxSphereBounds = Sphere<float>(center,radius);

	isLoadedComplete = true;
	bufferSize = 0;
	return;
#endif // 
}

void Mesh::processMesh(aiMesh* mesh,const aiScene* scene)
{
	scene; OPTICK_EVENT();
	std::vector<Vertex> mdlVertices;
	mdlVertices.reserve(mesh->mNumVertices);
	std::vector<uint32_t> mdlIndicies;
	mdlIndicies.reserve(mesh->mNumFaces * 3);

	std::string name = mesh->mName.C_Str();
	std::wstring wname = Helpers::string_cast<std::wstring>(name).c_str();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		auto position = Vector3f(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		auto color = Vector4f(
			RandomEngine::RandomInRange<float>(0,1),
			RandomEngine::RandomInRange<float>(0,1),
			RandomEngine::RandomInRange<float>(0,1),
			1.0f
		);

		//auto boneId = Vector4<unsigned int>(
		//	vert.BoneIDs[0],
		//	vert.BoneIDs[1],
		//	vert.BoneIDs[2],
		//	vert.BoneIDs[3]
		//);

		auto boneId = Vector4<unsigned int>(
			0,
			0,
			0,
			0
		);

		/*auto boneWeight = Vector4f(
			vert.BoneWeights[0],
			vert.BoneWeights[1],
			vert.BoneWeights[2],
			vert.BoneWeights[3],
		);*/

		auto boneWeight = Vector4f(
			0,
			0,
			0,
			0
		);

		auto UVCoord = Vector2f();
		if (mesh->HasTextureCoords(0))
		{
			UVCoord = Vector2f(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}

		auto normal = Vector3f();
		if (mesh->HasNormals())
		{
			normal = Vector3f(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);
		}

		auto tangent = Vector3f();
		if (mesh->HasTangentsAndBitangents())
		{
			tangent = Vector3f(
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z
			);
		}
		mdlVertices.emplace_back(
			position,
			color,
			UVCoord,
			normal,
			tangent,
			boneId,
			boneWeight
		);
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
			const auto& vertBoneData = mesh->mBones[i]->mWeights[j];
			mdlVertices[vertBoneData.mVertexId].AddBoneWeightAndID(vertBoneData.mWeight,i);
		}
	}

	auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue->GetCommandList();

	OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());

	VertexResource vertexRes(wname);
	if (!GPU::CreateVertexBuffer<Vertex>(commandList,vertexRes,mdlVertices))
	{
		std::cout << "Failed to create vertex buffer" << std::endl;
		return;
	}

	IndexResource indexRes(wname);
	if (!GPU::CreateIndexBuffer(commandList,indexRes,mdlIndicies))
	{
		std::cout << "Failed to create vertex buffer" << std::endl;
		return;
	}

	auto fenceValue = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fenceValue);
	commandQueue->Flush();

	Element toAdd;

	toAdd.VertexBuffer = vertexRes;
	toAdd.IndexResource = indexRes;
	toAdd.Vertices = mdlVertices;
	toAdd.Indicies = mdlIndicies;
	toAdd.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	toAdd.Stride = sizeof(Vertex);
	toAdd.MaterialIndex = mesh->mMaterialIndex;
	idToMaterial.try_emplace(mesh->mMaterialIndex,"");

	Elements.emplace_back(toAdd);
	mdlVertices.clear();
	mdlIndicies.clear();

	return;
}

void Mesh::ResizeBuffer()
{
	//D3D11_SUBRESOURCE_DATA vertexSubResourceData{};
	//vertexSubResourceData.pSysMem = myInstances.data();
	//vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Matrix) * myInstances.size());
	//HRESULT result;
	//result = RHI::Device->CreateBuffer(
	//	&vertexBufferDesc,
	//	&vertexSubResourceData,
	//	myInstanceBuffer.GetAddressOf()
	//);
	//
	//if (FAILED(result))
	//{
	//	Logger::Log("Failed to create Instance buffer");
	//	return;
	//}
	//bufferSize = static_cast<int>(myInstances.size());

}

void Mesh::UpdateInstanceBuffer()
{
	OPTICK_EVENT();
	//if (myInstances.size() > bufferSize)
	//{
	//	ResizeBuffer();
	//}
	//D3D11_MAPPED_SUBRESOURCE mappedResource{};
	//RHI::Context->Map(myInstanceBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedResource);
	//memcpy(mappedResource.pData,myInstances.data(),sizeof(Matrix) * myInstances.size());
	//RHI::Context->Unmap(myInstanceBuffer.Get(),0);
}
