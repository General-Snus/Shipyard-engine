#pragma once
#include <unordered_map>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
class InstanceRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void AddInstance(const RenderData& aRenderData);
	void UpdateInstance(const RenderData& aRenderData,const Matrix& aMatrix); 
private:
	std::unordered_map<RenderData&,std::vector<Matrix>> instanceRenderData;
};