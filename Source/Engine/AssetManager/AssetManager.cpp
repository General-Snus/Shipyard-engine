#include "AssetManager.pch.h"

#include "Objects/BaseAssets/MasterIncludeAssets.h"

std::shared_ptr<AssetBase> EngineResourcesLoader::TryLoadAsset(const std::filesystem::path &path)
{
    const auto assetTypeByExtension = AssetType(path);

    if (assetTypeByExtension == refl::reflect<Mesh>().name.str())
    {
        return LoadAsset<Mesh>(path);
    }

    if (assetTypeByExtension == refl::reflect<Material>().name.str())
    {
        return LoadAsset<Material>(path);
    }

    if (assetTypeByExtension == refl::reflect<ShipyardShader>().name.str())
    {
        return LoadAsset<ShipyardShader>(path);
    }

    if (assetTypeByExtension == refl::reflect<Animation>().name.str())
    {
        return LoadAsset<Animation>(path);
    }

    if (assetTypeByExtension == refl::reflect<Skeleton>().name.str())
    {
        return LoadAsset<Skeleton>(path);
    }

    if (assetTypeByExtension == refl::reflect<TextureHolder>().name.str())
    {
        return LoadAsset<TextureHolder>(path);
    }

    return std::shared_ptr<AssetBase>();
}
