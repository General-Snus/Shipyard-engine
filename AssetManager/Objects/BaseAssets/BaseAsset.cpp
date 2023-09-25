#include "AssetManager.pch.h"
#include "BaseAsset.h"
AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath),isLoadedComplete(false)
{
} 