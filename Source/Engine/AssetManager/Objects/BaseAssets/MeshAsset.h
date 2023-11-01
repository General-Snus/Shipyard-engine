#pragma once
#include "BaseAsset.h"
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp> 

class Mesh : public AssetBase
{
public:
	Mesh() = default;
	Mesh(const std::filesystem::path& aFilePath);
	void Init() override;

	std::vector<Element> Elements;
	//std::vector<Vertex> VertexData;
	//std::vector<unsigned int> IndexData;
	void UpdateInstanceBuffer();

	ComPtr<ID3D11Buffer> myInstanceBuffer;
	std::vector<CU::Matrix4x4<float>> myInstances;
	CU::Vector3<float> MaxBox;
	CU::Vector3<float> MinBox;
	CU::Sphere<float> boxSphereBounds;
};

