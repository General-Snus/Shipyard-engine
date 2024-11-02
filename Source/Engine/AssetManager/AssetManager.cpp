#include "AssetManager.pch.h"

#include "Objects/BaseAssets/MasterIncludeAssets.h"

#include <DirectX/DX12/Graphics/GPU.h>
#include <Tools/Utilities/Game/Timer.h>

void Library::ClearUnused()
{
    for (auto &i : content)
    {
        if (i.second && i.second->isLoadedComplete && !i.second->isBeingLoaded && i.second.use_count() == 1)
        {
            content.erase(i.first);
        }
    }
}

void ResourceLoaderBase::RecursiveNameSave()
{
    OPTICK_EVENT();
    nameToPathMap.clear();
    for (const auto &file : std::filesystem::recursive_directory_iterator(workingDirectory))
    {
        if (file.path().has_extension())
        {
            std::filesystem::path filePath = file.path();
            std::filesystem::path fileExt = file.path().extension();
            nameToPathMap.try_emplace(workingDirectory / filePath.filename(), filePath);
        }
    }
}
bool ResourceLoaderBase::AdaptPath(std::filesystem::path &path)
{
    OPTICK_EVENT();
    if (nameToPathMap.contains(path))
    {
        path = nameToPathMap.at(path);
        return true;
    }
    return false;
}
void ResourceLoaderBase::ClearUnused()
{
    for (auto &i : myLibraries)
    {
        i.second->ClearUnused();
    }
   // GPUInstance.m_GraphicsMemory->GarbageCollect();
}
void ResourceLoaderBase::ThreadedLoading()
{
    OPTICK_THREAD("ThreadedLoading");
    OPTICK_EVENT();

    if (myAssetQueue.GetSize())
    {
        {
            std::shared_ptr<AssetBase> working;
            working = myAssetQueue.Dequeue();

            if (!working)
            {
                Logger.Err("Something shit itself when loading asset: Asset was empty!");
                return;
            }

            working->isBeingLoaded = true;
            working->Init();

            const auto name = working->GetAssetPath().string().c_str();
            OPTICK_TAG("AssetType", name);

            if (working->isLoadedComplete)
            {
                working->isBeingLoaded = false;
                return;
            }

            working->isBeingLoaded = false;
            const std::string str = "Failed to load " + working->GetAssetPath().string();
            Logger.Warn(str);
        }
    }
}
std::string ResourceLoaderBase::AssetType(const std::filesystem::path &path)
{
    auto extension = path.extension().string();
    std::ranges::for_each(extension, [](char &c) { c = (char)std::tolower((int)c); });

    if (extension == "") // Either Invalid Or directory, either case ignore
    {
        return "";
    }
    else if (extension == ".fbx")
    {
        // Is Animation Test return "Animation"
        return refl::reflect<Mesh>().name.str();
    }
    else if (extension == ".json")
    {
        return refl::reflect<Material>().name.str();
        ;
    }
    else if (extension == ".cso" || extension == ".hlsl")
    {
        return refl::reflect<ShipyardShader>().name.str();
    }
    else if (extension == ".png" || extension == ".dds" || extension == ".hdr" || extension == ".jpg")
    {
        return refl::reflect<TextureHolder>().name.str();
    }

    return extension;
}

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
