#include "Engine/AssetManager/AssetManager.pch.h"

#include "Engine/AssetManager/Objects/BaseAssets/MasterIncludeAssets.h"

#include <Tools/Utilities/Game/Timer.h>

AssetManager::AssetManager()
{
	RecursiveNameSave();
}

void AssetManager::RecursiveNameSave()
{
	OPTICK_EVENT();
	for (const auto& file : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		if (file.path().has_extension())
		{
			std::filesystem::path filePath = file.path();
			std::filesystem::path fileExt = file.path().extension();
			nameToPathMap.try_emplace(AssetPath / filePath.filename(),filePath);
		}
	}

}
bool AssetManager::AdaptPath(std::filesystem::path& path)
{
	OPTICK_EVENT();
	if (nameToPathMap.contains(path))
	{
		path = nameToPathMap.at(path);
		return true;
	}
	return false;
}

std::string AssetManager::AssetType(const std::filesystem::path& path)
{
	auto  extension = path.extension().string();
	std::ranges::for_each(extension,[](char& c) { c = (char)std::tolower((int)c); });

	if (extension == "") //Either Invalid Or directory, either case ignore
	{
		return "";
	}
	else if (extension == ".fbx")
	{
		//Is Animation Test return "Animation"
		return refl::reflect<Mesh>().name.str();
	}
	else if (extension == ".json")
	{
		return refl::reflect<Material>().name.str();;
	}
	else if (extension == ".cso")
	{
		return refl::reflect<ShipyardShader>().name.str();
	}
	else if (extension == ".png" || extension == ".dds" || extension == ".hdr")
	{
		return refl::reflect<TextureHolder>().name.str();
	}

	return extension;
}

std::shared_ptr<AssetBase> AssetManager::TryLoadAsset(const std::filesystem::path& path)
{
	const auto assetTypeByExtension = AssetType(path);
	  
	if (assetTypeByExtension == refl::reflect<Mesh>().name.str())
	{
		return Get().LoadAsset<Mesh>(path);
	}

	if (assetTypeByExtension == refl::reflect<Material>().name.str())
	{
		return Get().LoadAsset<Material>(path);
	}

	if (assetTypeByExtension == refl::reflect<ShipyardShader>().name.str())
	{
		return Get().LoadAsset<ShipyardShader>(path);
	}

	if (assetTypeByExtension == refl::reflect<Animation>().name.str())
	{
		return Get().LoadAsset<Animation>(path);
	}

	if (assetTypeByExtension == refl::reflect<Skeleton>().name.str())
	{
		return Get().LoadAsset<Skeleton>(path);
	}

	if (assetTypeByExtension == refl::reflect<TextureHolder>().name.str())
	{
		return Get().LoadAsset<TextureHolder>(path);
	}

	return std::shared_ptr<AssetBase>();
}

void AssetManager::ThreadedLoading()
{
	OPTICK_THREAD("ThreadedLoading");
	OPTICK_EVENT();

	if (myAssetQueue.GetSize())
	{
		const double timeStart = Timer::GetTotalTime();
		{
			std::shared_ptr<AssetBase> working;
			working = myAssetQueue.Dequeue();

			if (!working)
			{
				Logger::Err("Something shit itself when loading asset: Asset was empty!");
				return;
			}

			working->isBeingLoaded = true;
			working->Init();

			const auto name = working->GetAssetPath().string().c_str();
			OPTICK_TAG("AssetType",name);

			if (working->isLoadedComplete)
			{
				//this->assetCallbackMaster.UpdateStatusOf<T>(working->GetAssetPath(),AssetCallbackMaster::created); 
				const double timeEnd = Timer::GetTotalTime();
				const double diff = (timeEnd - timeStart) * 1000.0;
				const std::string str = "Loaded: " + working->GetAssetPath().string() + " in " + std::to_string(diff) + "ms \n";
				Logger::Log(str);
				working->isBeingLoaded = false;
				return;
			}

			working->isBeingLoaded = false;
			const std::string str = "Failed to load " + working->GetAssetPath().string();
			Logger::Warn(str);
		}
	}
}




