#include "PersistentSystems.pch.h"

#include "SceneUtilities.h"
#include <Engine/AssetManager/AssetManager.h>

#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h>

bool SceneUtils::AddAssetToScene(const std::filesystem::path& aPath, const std::shared_ptr<Scene>& aScene)
{
	const std::string type = ENGINE_RESOURCES.AssetType(aPath);

	if (type == "")
	{
		LOGGER.Warn(std::format("Asset could not be injected into scene at {}", aPath.string()));
		return false;
	}
	GameObject obj = GameObject::Create("RenderedObject", aScene);
	obj.SetName(aPath.stem().string());
	obj.AddComponent<Transform>();

	auto& meshRend = obj.AddComponent<MeshRenderer>();

	if (type == refl::reflect<Mesh>().name.str())
	{
		meshRend.SetNewMesh(aPath);
	}

	else if (type == refl::reflect<Material>().name.str())
	{
		meshRend.SetMaterialPath(aPath);
	}

	else if (type == refl::reflect<ShipyardShader>().name.str())
	{
	}
	else
	{
		LOGGER.Warn(std::format("Asset could not be injected into scene at {}", aPath.string()));
		return false;
	}

	return false;
}

bool SceneUtils::AddAssetToObject(const std::filesystem::path& aPath, GameObject& aScene)
{
	const std::string type = ENGINE_RESOURCES.AssetType(aPath);

	if (type == "")
	{
		LOGGER.Warn(std::format("Asset could not be injected into scene at {}", aPath.string()));
		return false;
	}
	aScene.AddComponent<Transform>();

	auto& meshRend = aScene.AddComponent<MeshRenderer>();

	if (type == refl::reflect<Mesh>().name.str())
	{
		meshRend.SetNewMesh(aPath);
	}

	else if (type == refl::reflect<Material>().name.str())
	{
		meshRend.SetMaterialPath(aPath);
	}

	else if (type == refl::reflect<ShipyardShader>().name.str())
	{
		// aScene.AddComponent<ShipyardShader>();
	}
	else
	{
		LOGGER.Warn(std::format("Asset could not be injected into scene at {}", aPath.string()));
		return false;
	}

	return false;
}
