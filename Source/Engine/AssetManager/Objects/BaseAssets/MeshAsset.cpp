#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>

#include "MeshAsset.h" 
#include <Game/Modelviewer/Core/Modelviewer.h>
#include <Tools/Utilities/Math.hpp>

std::vector<std::string> GetTextureNames(const TGA::FBX::Material& material)
{
	std::vector<std::string> textureNames;

	if(material.Diffuse.Name != "")
	{
		textureNames.push_back(((std::filesystem::path)material.Diffuse.Name).replace_extension(".dds").string());
	}
	if(material.NormalMap.Name != "")
	{
		textureNames.push_back(((std::filesystem::path)material.NormalMap.Name).replace_extension(".dds").string());
	}
	if(material.Bump.Name != "")
	{
		textureNames.push_back(((std::filesystem::path)material.Bump.Name).replace_extension(".dds").string());
	}

	if(material.Specular.Name != "")
	{
		textureNames.push_back(((std::filesystem::path)material.Specular.Name).replace_extension(".dds").string());
	}
	return textureNames;
}

Mesh::Mesh(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{

}

void Mesh::Init()
{
	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Mesh inMesh;

	if(!std::filesystem::exists(AssetPath))
	{
		assert(false && "Mesh file does not exist");
	}

	if(TGA::FBX::Importer::LoadMeshW(AssetPath,inMesh))
	{
		Vector3f greatestExtent = {
			std::abs(inMesh.BoxSphereBounds.Center[0] - inMesh.BoxSphereBounds.BoxExtents[0]),
			std::abs(inMesh.BoxSphereBounds.Center[1] - inMesh.BoxSphereBounds.BoxExtents[1]),
			std::abs(inMesh.BoxSphereBounds.Center[2] - inMesh.BoxSphereBounds.BoxExtents[2])
		};

		float radius = 0;
		for(int i = 0; i < 3; ++i)
		{
			if(radius < greatestExtent[i])
			{
				radius = greatestExtent[i];
			}
		}

		boxSphereBounds = CU::Sphere<float>(
			{
				inMesh.BoxSphereBounds.Center[0],
				inMesh.BoxSphereBounds.Center[1],
				inMesh.BoxSphereBounds.Center[2]
			},
			radius
		);

		ModelViewer::Get().ExpandWorldBounds(boxSphereBounds);
		std::vector<Vertex> mdlVertices;
		std::vector<unsigned int> mdlIndicies;

		for(const auto& element : inMesh.Elements)
		{
			for(const auto& vert : element.Vertices)
			{
				auto position = CU::Vector3<float>(
					vert.Position[0],
					vert.Position[1],
					vert.Position[2]
				);
				auto color = CU::Vector4<float>(
					RandomInRange<float>(0,1),
					RandomInRange<float>(0,1),
					RandomInRange<float>(0,1),
					1.0f
				);

				auto boneId = CU::Vector4<unsigned int>(
					vert.BoneIDs[0],
					vert.BoneIDs[1],
					vert.BoneIDs[2],
					vert.BoneIDs[3]
				);

				auto boneWeight = CU::Vector4<float>(
					vert.BoneWeights[0],
					vert.BoneWeights[1],
					vert.BoneWeights[2],
					vert.BoneWeights[3]
				);

				auto UVCoord = CU::Vector2<float>(
					vert.UVs[0][0],
					vert.UVs[0][1]
				);

				auto normal = CU::Vector3<float>(
					vert.Normal[0],
					vert.Normal[1],
					vert.Normal[2]
				);

				auto tangent = CU::Vector3<float>(
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

			VertexData.insert(VertexData.end(),mdlVertices.begin(),mdlVertices.end());
			IndexData.insert(IndexData.end(),mdlIndicies.begin(),mdlIndicies.end());

			ComPtr<ID3D11Buffer> vertexBuffer;
			// After all not, Why shouldnt i keep the instances for my self?
			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mdlVertices.size());
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = mdlVertices.data();


			auto result = RHI::Device->CreateBuffer(&vertexBufferDesc,&vertexData,vertexBuffer.GetAddressOf());
			if(FAILED(result))
			{
				AMLogger.Err("Failed to create mesh	 Instance buffer");
				return;
			}
			//if(!RHI::CreateVertexBuffer<Vertex>(vertexBuffer,mdlVertices))
			//{
			//	std::cout << "Failed to create vertex buffer" << std::endl;
			//	return;
			//}
			ComPtr<ID3D11Buffer> indexBuffer;
			if(!RHI::CreateIndexBuffer(indexBuffer,mdlIndicies))
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
	MaxBox = CU::Vector3<float>(inMesh.BoxBounds.Max[0],inMesh.BoxBounds.Max[1],inMesh.BoxBounds.Max[2]);
	MinBox = CU::Vector3<float>(inMesh.BoxBounds.Min[0],inMesh.BoxBounds.Min[1],inMesh.BoxBounds.Min[2]);
	isLoadedComplete = true;
}
