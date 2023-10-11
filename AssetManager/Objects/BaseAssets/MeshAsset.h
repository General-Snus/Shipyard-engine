#pragma once
#include "BaseAsset.h"
#include <ThirdParty/CU/CommonUtills/Sphere.hpp> 

class Mesh : public AssetBase
{
public:
	Mesh() = default;
	Mesh(const std::filesystem::path& aFilePath);
	void Init() override;

	std::vector<Element> Elements;
	std::vector<Vertex> VertexData;
	std::vector<unsigned int> IndexData;

	CU::Vector3<float> MaxBox;
	CU::Vector3<float> MinBox;
	CU::Sphere<float> boxSphereBounds;
};

