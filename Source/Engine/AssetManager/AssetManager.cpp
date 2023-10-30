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
}

AssetManager& AssetManager::GetInstance()
{
	static AssetManager singleton;
	return singleton;
}
void AssetManager::ThreadedLoading()
{
	if(myAssetQueue.GetSize())
	{
		const double timeStart = CU::Timer::GetInstance().GetTotalTime();	
		std::scoped_lock<std::mutex> lock(lockForSet);

		std::shared_ptr<AssetBase> working = myAssetQueue.Dequeue();
		working->Init();
		working->isLoadedComplete = true;
		const double timeEnd = CU::Timer::GetInstance().GetTotalTime();
		const double diff = (timeEnd - timeStart)*1000.0;
		std::string str = "Loaded: " + working->AssetPath.string() + " in " + std::to_string(diff) + "ms \n";
		AMLogger.Log(str);
	}
}




