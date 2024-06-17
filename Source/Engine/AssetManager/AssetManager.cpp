#include "AssetManager.pch.h" 
#include <filesystem>

#include <algorithm>
#include <cctype>
#include <string>
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

			working->isBeingLoaded = true;
			working->Init();

			const auto name = working->GetAssetPath().string().c_str();
			OPTICK_TAG("AssetType",name);

			if (working->isLoadedComplete)
			{
				working->isBeingLoaded = false;
				//this->assetCallbackMaster.UpdateStatusOf<T>(working->GetAssetPath(),AssetCallbackMaster::created); 
				const double timeEnd = Timer::GetTotalTime();
				const double diff = (timeEnd - timeStart) * 1000.0;
				const std::string str = "Loaded: " + working->GetAssetPath().string() + " in " + std::to_string(diff) + "ms \n";
				Logger::Log(str);
				return;
			}
			const std::string str = "Failed to load " + working->GetAssetPath().string();
			Logger::Warn(str);
		}
	}
}




