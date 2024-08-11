#pragma once 
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>  
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>/*

struct renderDataHash
{
	size_t operator()(const std::shared_ptr<Mesh>& aMesh) const
	{
		return std::hash<std::shared_ptr<Mesh>>()(aMesh);
	}
};

class InstanceRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute(bool isShadowPass);
	void AddInstance(const std::shared_ptr<Mesh>& aRenderData);

	void Clear();
private:

	std::unordered_map<std::shared_ptr<Mesh>,> instanceRenderData;
};*/