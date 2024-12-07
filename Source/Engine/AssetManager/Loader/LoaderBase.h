#pragma once
#include "Tools/Utilities/DataStructures/Queue.hpp" 
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>  
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <unordered_map>

template <class T> struct LoadTask
{
	LoadTask(const std::filesystem::path &aPath, const bool useExact, std::shared_ptr<T> &outAsset)
		: path(aPath), ExactPath(useExact), asset(outAsset)
	{
	}
	std::filesystem::path path;
	bool ExactPath;
	std::shared_ptr<T> asset;
};

class Library
{
	using assetMap = std::unordered_map<std::filesystem::path, std::shared_ptr<AssetBase>>;
	friend class ResourceLoaderBase;
	friend class EngineResourcesLoader;
	friend class GameResourcesLoader;

  public:
	Library() = default;
	template <class T> std::shared_ptr<T> Add(const std::pair<std::filesystem::path, std::shared_ptr<T>> &pair);
	template <class T> std::shared_ptr<T> Get(const std::filesystem::path &aFilePath);
	bool Has(const std::filesystem::path &aFilePath) const;

	template <class T> std::map<std::filesystem::path, std::shared_ptr<T>> GetContentCatalogue();

  private:
	void ClearUnused();

  private:
	assetMap content;
};

inline bool Library::Has(const std::filesystem::path &aFilePath) const
{
	return (content.contains(aFilePath) && content.at(aFilePath));
}

template <class T> std::shared_ptr<T> Library::Add(const std::pair<std::filesystem::path, std::shared_ptr<T>> &pair)
{
	content[pair.first] = std::static_pointer_cast<AssetBase>(pair.second);
	return std::static_pointer_cast<T>(pair.second);
}

template <class T> std::shared_ptr<T> Library::Get(const std::filesystem::path &aFilePath)
{
	return std::dynamic_pointer_cast<T>(content[aFilePath]);
}

template <class T> std::map<std::filesystem::path, std::shared_ptr<T>> Library::GetContentCatalogue()
{
	// TODO what was i cooking here??
	std::map<std::filesystem::path, std::shared_ptr<T>> newOutMap;
	for (auto &i : content)
	{
		newOutMap.emplace(i.first, std::dynamic_pointer_cast<T>(i.second));
	}
	return newOutMap;
}

class ResourceLoaderBase : public Singleton
{
  public:
	// Shame place, these are needed for the hopefully nieche condition of not knowing what you are loading
	std::string AssetType(const std::filesystem::path &path);

	// CAREFUL YOU FOOL
	template <class T> std::shared_ptr<Library> GetLibraryOfType();

	void SubscribeToChanges(const std::filesystem::path &aFilePath, SY::UUID gameobjectID);
	bool AdaptPath(std::filesystem::path &path);
	void ClearUnused();
	void RecursiveNameSave();

	void SetWorkingDirectory(const std::filesystem::path &path);

	// TODO make getter
	const std::filesystem::path &Directory() const
	{
		return workingDirectory;
	}

  protected:
	std::filesystem::path workingDirectory;
	void ThreadedLoading();
	Queue<std::shared_ptr<AssetBase>> myAssetQueue;
	std::unordered_map<std::filesystem::path, std::filesystem::path> nameToPathMap;
	std::unordered_map<const std::type_info *, std::shared_ptr<Library>> myLibraries;
};

template <class T> std::shared_ptr<Library> ResourceLoaderBase::GetLibraryOfType()
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	const auto it = myLibraries.find(typeInfo);
	if (it != myLibraries.end())
	{
		return it->second;
	}
	return nullptr;
}

inline void ResourceLoaderBase::SetWorkingDirectory(const std::filesystem::path &path)
{
	workingDirectory = path;
}