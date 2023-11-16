#pragma once
#include "BaseAsset.h"
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp> 

struct aiMesh;
struct aiScene;
#define UseTGAImporter 0
class Mesh : public AssetBase
{
public:
	Mesh() = default;
	Mesh(const std::filesystem::path& aFilePath);
	void Init() override;

	void processMesh(aiMesh* mesh,const aiScene* scene);

	void ResizeBuffer();

	std::vector<Element> Elements;
	//std::vector<Vertex> VertexData;
	//std::vector<unsigned int> IndexData;
	void UpdateInstanceBuffer();

	int bufferSize;
	ComPtr<ID3D11Buffer> myInstanceBuffer;//TODO unique id meshrender if dirty remake? 
	D3D11_BUFFER_DESC vertexBufferDesc{};
	std::vector< Matrix > myInstances;
	Vector3f MaxBox;
	Vector3f MinBox;
	Sphere<float> boxSphereBounds;
};

