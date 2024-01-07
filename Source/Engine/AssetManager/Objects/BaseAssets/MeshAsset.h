#pragma once
#include "BaseAsset.h"
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp> 

struct aiMesh;
struct aiScene;
class Mesh : public AssetBase
{
public:
	friend class cMeshRenderer;
	friend class InstanceRenderer;
	Mesh() = default;
	Mesh(const std::filesystem::path& aFilePath);
	void Init() override; 

	Vector3f MaxBox;
	Vector3f MinBox;
	Sphere<float> boxSphereBounds;
	std::vector<Element> Elements; 
	const std::unordered_map<unsigned int,std::shared_ptr<Material>>& GetMaterialList();
private:
	std::unordered_map<unsigned int,std::shared_ptr<Material>> materials;
	std::unordered_map<unsigned int,std::filesystem::path> idToMaterial;
	ComPtr<ID3D11Buffer> myInstanceBuffer;//TODO unique id meshrender if dirty remake? 
	D3D11_BUFFER_DESC vertexBufferDesc{};
	std::vector<Matrix> myInstances;
	int bufferSize;
	void processMesh(aiMesh* mesh,const aiScene* scene); 
	void ResizeBuffer(); 
	void UpdateInstanceBuffer();
};

