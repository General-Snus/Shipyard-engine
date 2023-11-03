#pragma once
#include <unordered_set>
#include <set>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
using pairData = std::pair<std::string,RenderData*>;
template<>
struct std::hash<pairData>
{
	size_t operator()(const pairData& aPair) const
	{
		return std::hash<std::string>()(aPair.first);
	}
};

class InstanceRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void AddInstance(RenderData* aRenderData);
private:

	//std::unordered_set<std::pair<std::shared_ptr<Mesh>,std::vector<std::weak_ptr<Material>>>> instanceRenderData;
	std::unordered_set<pairData,std::hash<pairData>> instanceRenderData;
};