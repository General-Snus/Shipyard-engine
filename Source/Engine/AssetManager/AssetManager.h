#pragma once
#define AsUINT(v) static_cast<unsigned>(v)

#include "Loader/LoaderBase.h" 
#include <Editor/Editor/Defines.h>
#include <Engine/AssetManager/ComponentSystem/UUID.h>
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/System/ThreadPool.hpp>

#define EngineResources ServiceLocator::Instance().GetService<EngineResourcesLoader>()

class EngineResourcesLoader : public ResourceLoaderBase
{
  public:
	template <class T> bool HasAsset(const std::filesystem::path &aFilePath, bool useExact) const;
	template <class T> std::shared_ptr<T> LoadAsset(const std::filesystem::path &aFilePath);
	template <class T> std::shared_ptr<T> LoadAsset(const std::filesystem::path &aFilePath, bool useExact);
	template <class T> bool ForceLoadAsset(const std::filesystem::path &aFilePath, std::shared_ptr<T> &outAsset);
	template <class T>
	bool ForceLoadAsset(const std::filesystem::path &aFilePath, bool useExact, std::shared_ptr<T> &outAsset);
	std::shared_ptr<AssetBase> TryLoadAsset(const std::filesystem::path &path);
};

template <class T>
bool EngineResourcesLoader::ForceLoadAsset(const std::filesystem::path &aFilePath, std::shared_ptr<T> &outAsset)
{
	return ForceLoadAsset<T>(aFilePath, false, outAsset);
}

template <class T>
bool EngineResourcesLoader::ForceLoadAsset(const std::filesystem::path &identifierPath, const bool useExact,
										   std::shared_ptr<T> &outAsset)
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}

	std::filesystem::path Identifier = identifierPath;
	std::filesystem::path loadObjectFrom = identifierPath;

	if (useExact)
	{
		Identifier = std::filesystem::relative(identifierPath, workingDirectory); // Might invoke unexpected behaviour
																				  // but stops you from duplication

		// TODO FIgure out if use exact is even a good thing to have? Can i automate
		// without any sliver of doubt?
	}
	else
	{
		loadObjectFrom = workingDirectory / loadObjectFrom;
	}

	std::shared_ptr<T> ptr = library->Get<T>(Identifier);

	if (!ptr || !ptr->isLoadedComplete)
	{
		std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
		ptr = library->Add(newObject);
		newObject.second->Init();
	}
	outAsset = ptr;

	return outAsset->isLoadedComplete;
}

// Is threaded, dont expect immediate results
template <class T> std::shared_ptr<T> EngineResourcesLoader::LoadAsset(const std::filesystem::path &aFilePath)
{
	return LoadAsset<T>(aFilePath, false);
}

// Is threaded, dont expect immediate results
template <class T>
std::shared_ptr<T> EngineResourcesLoader::LoadAsset(const std::filesystem::path &identifierPath, const bool useExact)
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}
	std::filesystem::path Identifier = identifierPath;
	std::filesystem::path loadObjectFrom = identifierPath;

	if (useExact)
	{
		Identifier = std::filesystem::relative(identifierPath, workingDirectory);
	}
	else
	{
		loadObjectFrom = workingDirectory / loadObjectFrom;
	}

	std::shared_ptr<T> ptr = library->Get<T>(Identifier);

	if (!ptr)
	{
		std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
		ptr = library->Add(newObject);
		myAssetQueue.EnqueueUnique(newObject.second);
		newObject.second->isBeingLoaded = true;
#if ThreadedAssetLoading
		ThreadPoolInstance.SubmitWork(std::bind(&EngineResourcesLoader::ThreadedLoading, this));
#else
		ThreadedLoading();
#endif
	}
	return ptr;
}

template <class T>
bool EngineResourcesLoader::HasAsset(const std::filesystem::path &identifierPath, const bool useExact) const
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	const std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		return false;
	}
	std::filesystem::path Identifier = identifierPath;
	std::filesystem::path loadObjectFrom = identifierPath;

	if (useExact)
	{
		Identifier = std::filesystem::relative(identifierPath, workingDirectory);
	}
	else
	{
		loadObjectFrom = workingDirectory / loadObjectFrom;
	}

	return library->Has(Identifier);
}
