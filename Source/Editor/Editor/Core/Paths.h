#pragma once
#include <filesystem>


namespace paths
{
	// Returns the path to the executable .exe
	std::filesystem::path GetExecutablePath();
	std::filesystem::path GetMainAssetDir();
}

