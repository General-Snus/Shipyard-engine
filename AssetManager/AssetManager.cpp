#include "AssetManager.pch.h"
#include <filesystem>
#include <GraphicsEngine/Objects/Shader.h>
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>

#include <algorithm>
#include <cctype>
#include <string>

inline AssetManager::AssetManager()
{

}

AssetManager::~AssetManager()
{
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
	lockForSet.lock();
		std::shared_ptr<AssetBase> working = myAssetQueue.Dequeue(); 
		working->Init();
		working->isLoadedComplete = true;
	lockForSet.unlock();
		std::cout << "Loaded: " << working->AssetPath << "\n\n";
	}
}




