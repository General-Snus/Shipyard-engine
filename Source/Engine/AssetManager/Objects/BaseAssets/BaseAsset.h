#pragma once 
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <Tools/Utilities/LinearAlgebra//Sphere.hpp> 
#include <filesystem>
#include <functional>
class Material;


class AssetBase
{
public:  
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	virtual void Init() = 0;
	bool isLoadedComplete = false;
	inline const std::filesystem::path& GetAssetPath() const { return AssetPath;	};
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
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;
	UINT NumVertices = 0;
	UINT NumIndices = 0;
	UINT PrimitiveTopology = 0;
	UINT Stride = 0;
	unsigned int MaterialIndex = 0;
};
