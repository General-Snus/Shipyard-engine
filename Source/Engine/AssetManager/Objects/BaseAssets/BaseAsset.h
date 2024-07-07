#pragma once 
#include <Engine/AssetManager/Enums.h>
#include "DirectX/Shipyard/GpuResource.h"
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h"
#include "Tools/Reflection/refl.hpp"
#include "Tools/Logging/Logging.h"
#include "Engine/GraphicsEngine/Rendering/Vertex.h"
#include <functional> 
 
class Scene;

class AssetBase : public Reflectable
{
public:
	MYLIB_REFLECTABLE();
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	//When overriding you have the responisiblitity to set the isloadedcomplete flag
	virtual void Init() = 0;
	const std::filesystem::path& GetAssetPath()	const { return AssetPath; };
	bool InspectorView() override;

	virtual bool InjectIntoScene(std::shared_ptr<Scene> SceneToAddAsset) 
	{
		Logger::Warn(std::format("Asset could not be added to scene\nAsset of type {} is not possible to load into scene",GetTypeInfo().Name())); 
		return false; 
	};

	std::filesystem::path AssetPath;
	bool  isLoadedComplete = false;
	bool  isBeingLoaded = false;
	std::vector<std::function<void()>> callBackOnFinished; 
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