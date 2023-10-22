#include "AssetManager.pch.h"
#include <filesystem>
#include <Engine/GraphicsEngine/Objects/Shader.h>
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

#include <algorithm>
#include <cctype>
#include <string>
 
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

		std::scoped_lock<std::mutex> lock(lockForSet);

		std::shared_ptr<AssetBase> working = myAssetQueue.Dequeue();
		working->Init();
		working->isLoadedComplete = true; 
		std::string str = "Loaded: " + working->AssetPath.string() + "\n\n";
		std::cout << str;
	}
}




