#pragma once
#include <unordered_set>
#include <set>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>  

struct renderDataHash
{
	size_t operator()(const std::shared_ptr<RenderData>& aPair) const
	{ 
		return std::hash<std::shared_ptr<Mesh>>()(aPair->myMesh);
	}
};

class InstanceRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute(bool isShadowPass);
	void AddInstance(const std::shared_ptr<RenderData>& aRenderData);
	 
	void Clear();
private:
	 
	std::unordered_map<std::shared_ptr<Mesh>,std::shared_ptr<RenderData> > instanceRenderData;
};