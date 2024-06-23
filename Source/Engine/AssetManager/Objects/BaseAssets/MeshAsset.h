#pragma once
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp> 
#include "BaseAsset.h"
#include "Tools/Utilities/LinearAlgebra/AABB3D.hpp"
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"

class Material;
struct aiMesh;
struct aiScene;
class Mesh : public AssetBase
{
public:
	MYLIB_REFLECTABLE();
	friend class cMeshRenderer;
	friend class InstanceRenderer;
	Mesh() = default;
	Mesh(const std::filesystem::path& aFilePath);
	void Init() override;

	AABB3D<float> Bounds; 
	std::vector<Element> Elements;
	const std::unordered_map<unsigned int,std::shared_ptr<Material>>& GetMaterialList();
	void FillMaterialPaths(const aiScene* scene);
	void InspectorView() override;

private:
	std::unordered_map<unsigned int,std::shared_ptr<Material>> materials;
	std::unordered_map<unsigned int,std::filesystem::path> idToMaterial;

	ComPtr<ID3D12Resource> myInstanceBuffer{};
	std::vector<Matrix> myInstances{};
	int bufferSize{};
	void processMesh(aiMesh* mesh,const aiScene* scene);
	void ResizeBuffer();
	void UpdateInstanceBuffer();
};

REFL_AUTO(
	type(Mesh,bases<AssetBase>),
	field(Bounds)
)