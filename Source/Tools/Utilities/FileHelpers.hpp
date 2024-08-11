#pragma once
#include <filesystem>


inline bool IsPathInsideDirectory(const std::filesystem::path& path, const std::filesystem::path& directoryPath) {
	// Get the canonical absolute paths
	auto absPath = std::filesystem::canonical(path);
	auto absDirectoryPath = std::filesystem::canonical(directoryPath);

	// Use std::mismatch to compare the paths
	auto [pathIt, dirPathIt] = std::mismatch(absPath.begin(), absPath.end(), absDirectoryPath.begin(), absDirectoryPath.end());

	// If dirPathIt is at the end, all components matched, indicating the path is inside the directory
	return dirPathIt == absDirectoryPath.end();
}
