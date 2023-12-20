#include "AssetManager.pch.h"
#include "../BaseAsset.h"
#include <filesystem>
AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath),isLoadedComplete(false)
{
} 