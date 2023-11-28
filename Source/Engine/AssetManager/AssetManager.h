#pragma once

#define AsUINT(v) static_cast<unsigned>(v) 

#include <Tools/Utilities/DataStructures/Queue.hpp>
#include <thread>
#include <future>
#include <unordered_map>


struct Frame;
struct Element;
struct MaterialData;
struct Bone;
class Mesh;
class Animation;
class Skeleton;
class Material;


template<class T>
struct LoadTask
{
	LoadTask(const std::filesystem::path& aPath, bool useExact, std::shared_ptr<T>& outAsset)
		: path(aPath), ExactPath(useExact), asset(outAsset)
	{
	}
	std::filesystem::path path;
	bool ExactPath;
	std::shared_ptr<T> asset;
};


class Library
{
public:
	Library() = default;

	template<class T>
	std::shared_ptr<T> Add(const std::pair<std::filesystem::path, std::shared_ptr<T>>& pair);

	template<class T>
	std::shared_ptr<T> Get(const std::filesystem::path& aFilePath);

	template<class T>
	std::unordered_map <std::filesystem::path, std::shared_ptr<T>> GetContentCatalogue();
private:
	std::unordered_map <std::filesystem::path, std::shared_ptr<AssetBase>> content;
};

class AssetManager
{
public:
	AssetManager();
	~AssetManager() = default;
	static AssetManager& GetInstance();

	template<class T>
	void LoadAsset(const std::filesystem::path& aFilePath, std::shared_ptr<T>& outAsset);
	template<class T>
	void LoadAsset(const std::filesystem::path& aFilePath, bool useExact, std::shared_ptr<T>& outAsset);
	template<class T>
	void HasAsset(const std::filesystem::path& aFilePath, bool useExact) const; 
	template<class T>
	void ForceLoadAsset(const std::filesystem::path& aFilePath, std::shared_ptr<T>& outAsset);
	template<class T>
	void ForceLoadAsset(const std::filesystem::path& aFilePath, bool useExact, std::shared_ptr<T>& outAsset);

private:
	void ThreadedLoading();
	std::mutex lockForSet;
	std::vector<std::future<void>> myThreads;

	Queue<std::shared_ptr<AssetBase>> myAssetQueue;

	template<class T>
	std::shared_ptr<Library> GetLibraryOfType();

	std::unordered_map<const std::type_info*, std::shared_ptr<Library>> myLibraries;
	const std::filesystem::path AssetPath = L"../../Content/";
};
template<class T>
void AssetManager::ForceLoadAsset(const std::filesystem::path& aFilePath, std::shared_ptr<T>& outAsset)
{
	ForceLoadAsset<T>(aFilePath, false, outAsset);
}
template<class T>
void AssetManager::ForceLoadAsset(const std::filesystem::path& aFilePath, bool useExact, std::shared_ptr<T>& outAsset)
{
	const std::type_info* typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}

	std::shared_ptr<T> ptr = library->Get<T>(aFilePath);

	if (!ptr)
	{
		if (useExact)
		{
			std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(aFilePath, std::make_shared<T>(aFilePath));
			ptr = library->Add(newObject);
			newObject.second->Init();
		}
		else
		{
			std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(aFilePath, std::make_shared<T>(AssetPath / aFilePath));
			ptr = library->Add<T>(newObject);
			newObject.second->Init();
		}
	}
	outAsset = ptr;
}

/// <summary>
/// Holds the current thread until the asset is loaded
/// </summary> 
template<class T>
void AssetManager::LoadAsset(const std::filesystem::path& aFilePath, std::shared_ptr<T>& outAsset)
{
	LoadAsset<T>(aFilePath, false, outAsset);
}

/// <summary>
/// Holds the current thread until the asset is loaded
/// </summary> 
template<class T>
void AssetManager::LoadAsset(const std::filesystem::path& aFilePath, bool useExact, std::shared_ptr<T>& outAsset)
{
	const std::type_info* typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}

	std::shared_ptr<T> ptr = library->Get<T>(aFilePath);

	if (!ptr)
	{
		if (useExact)
		{
			std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(aFilePath, std::make_shared<T>(aFilePath));
			ptr = library->Add(newObject);
			myAssetQueue.EnqueueUnique(newObject.second);
			myThreads.push_back(std::async([this]() { this->ThreadedLoading(); }));
		}
		else
		{
			std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(aFilePath, std::make_shared<T>(AssetPath / aFilePath));
			ptr = library->Add<T>(newObject);
			myAssetQueue.EnqueueUnique(newObject.second);
			myThreads.push_back(std::async([this]() { this->ThreadedLoading(); }));
		}
	}
	outAsset = ptr;
}

template<class T>
inline void AssetManager::HasAsset(const std::filesystem::path& aFilePath, bool useExact) const
{
	aFilePath; useExact;
	//AMLogger.Err("NotImplementedException");
	assert(false);
}
 
template<class T>
inline std::shared_ptr<T> Library::Add(const std::pair<std::filesystem::path, std::shared_ptr<T>>& pair)
{
	content[pair.first] = std::static_pointer_cast<AssetBase>(pair.second);
	return std::static_pointer_cast<T>(pair.second);
}

template<class T>
inline std::shared_ptr<T> Library::Get(const std::filesystem::path& aFilePath)
{
	return  std::dynamic_pointer_cast<T>(content[aFilePath]);
}

template<class T>
inline std::unordered_map<std::filesystem::path, std::shared_ptr<T>> Library::GetContentCatalogue()
{
	std::unordered_map<std::filesystem::path, std::shared_ptr<T>> newOutMap;
	for (auto& i : content)
	{
		newOutMap.emplace(i.first,std::dynamic_pointer_cast<T>(i.second));
	}
	return newOutMap;
}

template<class T>
std::shared_ptr<Library> AssetManager::GetLibraryOfType()
{
	const std::type_info* typeInfo = &typeid(T);
	auto it = myLibraries.find(typeInfo);
	if (it != myLibraries.end())
	{
		return it->second;
	}
	return nullptr;
}
