#include "AssetManager.pch.h"

#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <wrl.h>
#include "../BaseAsset.h"

AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath),isLoadedComplete(false)
{
}