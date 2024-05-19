#pragma once 
#include <Engine/AssetManager/Enums.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <filesystem>
#include  <functional>
#include "DirectX/Shipyard/GpuResource.h"
#include "Tools/Reflection/refl.hpp"
#include "Tools/Reflection/ReflectionTemplate.h"

class Material;
class AssetBase : public Reflectable
{
public:
	MYLIB_REFLECTABLE();
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	virtual void Init() = 0;
	bool  isLoadedComplete = false;
	bool  isBeingLoaded = false;
	const std::filesystem::path& GetAssetPath()	const { return AssetPath; };
	std::vector<std::function<void()>> callBackOnFinished;


	virtual void InspectorView();
	std::filesystem::path AssetPath;
protected:
};

REFL_AUTO(type(AssetBase),
	field(AssetPath),
	field(isLoadedComplete),
	field(isBeingLoaded)
)

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