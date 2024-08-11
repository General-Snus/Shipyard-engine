#pragma once
#include <Engine/PersistentSystems/Scene.h>

namespace SceneUtils

{

	bool AddAssetToScene(const std::filesystem::path& aPath,const std::shared_ptr<Scene>& aAsset);

















	bool AddAssetToObject(const std::filesystem::path& aPath,  GameObject& aScene);

}