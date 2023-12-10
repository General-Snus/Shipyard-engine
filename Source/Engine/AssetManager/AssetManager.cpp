#include "AssetManager.pch.h"
#include <filesystem>
#include <Engine/GraphicsEngine/Objects/Shader.h>
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <Tools/Utilities/Game/Timer.h>

AssetManager::AssetManager()
{
	AMLogger = Logger::Create("AssetManager");
	AMLogger.SetPrintToVSOutput(false); 
	RecursiveNameSave();
}
 
void AssetManager::RecursiveNameSave()
{
	for(const auto& file : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		if(file.path().has_extension())
		{
			std::filesystem::path filePath = file.path();
			std::filesystem::path fileExt = file.path().extension();
			nameToPathMap.try_emplace(AssetPath/filePath.filename(),filePath);
		}	 
	}

}
bool AssetManager::AdaptPath(std::filesystem::path& path)
{
	if(nameToPathMap.contains(path))
	{
		path = nameToPathMap.at(path);
		return true;
	} 
	return false;
}
 
void AssetManager::ThreadedLoading()
{
	if(myAssetQueue.GetSize())
	{
		const double timeStart = Timer::GetInstance().GetTotalTime(); 
		{
			std::shared_ptr<AssetBase> working = myAssetQueue.Dequeue();
			working->Init(); 
			if(working->isLoadedComplete)
			{
				//this->assetCallbackMaster.UpdateStatusOf<T>(working->GetAssetPath(),AssetCallbackMaster::created); 
				const double timeEnd = Timer::GetInstance().GetTotalTime();
				const double diff = (timeEnd - timeStart) * 1000.0;
				std::string str = "Loaded: " + working->GetAssetPath().string() + " in " + std::to_string(diff) + "ms \n";
				AMLogger.Log(str); 
				return;
			}
			std::string str = "Failed to load " + working->GetAssetPath().string();
			AMLogger.Warn(str);
		}
	}
}




