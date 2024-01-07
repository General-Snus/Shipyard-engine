#include "AssetManager.pch.h"
#include "../BaseAsset.h"
#include <filesystem>

#include <d3d11.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <wrl.h>

AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath),isLoadedComplete(false)
{
}