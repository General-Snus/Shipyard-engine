#pragma once 
#include <Engine/AssetManager/Enums.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <filesystem>
#include  <functional>
#include "DirectX/Shipyard/GpuResource.h"

class Material;
class AssetBase
{
public:
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	virtual void Init() = 0;
	bool  isLoadedComplete = false;
	bool  isBeingLoaded = false;
	inline const std::filesystem::path& GetAssetPath()	const { return AssetPath; };
	std::vector<std::function<void()>> callBackOnFinished;
protected:
	std::filesystem::path AssetPath;
};

//struct Bone
//{
//	Matrix BindPoseInverse;
//	int ParentIdx = -1;
//	std::string Name;
//	std::vector<unsigned> Children;
//};
//
//
//struct Frame
//{
//	std::unordered_map<std::string,Matrix > myTransforms;
//};

struct Element
{
	VertexResource VertexBuffer;
	IndexResource IndexResource;
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indicies;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT Stride = 0;
	unsigned int MaterialIndex = 0;
};