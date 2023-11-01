#pragma once
#include <unordered_set>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
class InstanceRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void AddInstance(RenderData* aRenderData);
private:
	//std::unordered_set<std::pair<std::shared_ptr<Mesh>,std::vector<std::weak_ptr<Material>>>> instanceRenderData;
	std::unordered_set<RenderData*> instanceRenderData;
};