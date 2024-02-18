#pragma once 
#include <Engine/AssetManager/Enums.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <filesystem>
#include  <functional>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
class Material;
class AssetBase
{
public:
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	virtual void Init() = 0;
	bool isLoadedComplete = false;
	bool isBeingLoaded = false;
	inline const std::filesystem::path& GetAssetPath()	const { return AssetPath; };
	std::vector<std::function<void()>> callBackOnFinished;
protected:
	std::filesystem::path AssetPath;
};

struct Bone
{
	Matrix BindPoseInverse;
	int ParentIdx = -1;
	std::string Name;
	std::vector<unsigned> Children;
};


struct Frame
{
	std::unordered_map<std::string,Matrix > myTransforms;
};

struct Element
{
	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> IndexBuffer;
	std::vector<Vertex> Vertices;
	std::vector<unsigned int>Indicies;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	UINT NumVertices = 0;
	UINT NumIndices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT Stride = 0;
	unsigned int MaterialIndex = 0;
};