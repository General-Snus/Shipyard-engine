#pragma once
#include "Loader/LoaderBase.h" 
#include <Editor/Editor/Defines.h> 
#include <Tools/Utilities/System/ServiceLocator.h>
#define Resources ServiceLocator::Instance().GetService<GameResourcesLoader>()

class GameResourcesLoader : public ResourceLoaderBase
{
  public:
	template <class T> bool Find(const std::filesystem::path &aFilePath) const;
	template <class T> std::shared_ptr<T> Load(const std::filesystem::path &aFilePath);
	std::shared_ptr<AssetBase> TryLoad(const std::filesystem::path &path);
	template <class T> std::shared_ptr<T> ForceLoad(const std::filesystem::path &aFilePath);
	template <class T> std::shared_ptr<T> Create(const std::filesystem::path &aFilePath);
};

template <class T> bool GameResourcesLoader::Find(const std::filesystem::path &aFilePath) const
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		return false;
	}

	std::filesystem::path Identifier = aFilePath;
	std::filesystem::path loadObjectFrom = workingDirectory / aFilePath;

	return library->Has(Identifier);
}

template <class T> std::shared_ptr<T> GameResourcesLoader::Load(const std::filesystem::path &aFilePath)
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}
	std::filesystem::path Identifier = aFilePath;
	std::filesystem::path loadObjectFrom = workingDirectory / aFilePath;

	std::shared_ptr<T> ptr = library->Get<T>(Identifier);

	if (!ptr)
	{
		std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
		ptr = library->Add(newObject);
		myAssetQueue.EnqueueUnique(newObject.second);
		newObject.second->isBeingLoaded = true;
#if ThreadedAssetLoading
		ThreadPoolInstance.SubmitWork(std::bind(&ResourceLoaderBase::ThreadedLoading, this));
#else
		ThreadedLoading();
#endif
	}
	return ptr;
}

template <class T> std::shared_ptr<T> GameResourcesLoader::ForceLoad(const std::filesystem::path &aFilePath)
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}

	std::filesystem::path Identifier = aFilePath;
	std::filesystem::path loadObjectFrom = workingDirectory / aFilePath;

	auto ptr = (library->Get<T>(Identifier));

	if (!ptr || !ptr->isLoadedComplete)
	{
		std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
		ptr = library->Add(newObject);
		newObject.second->Init();
	}

	if (ptr->isLoadedComplete)
	{
		return ptr;
	}

	ptr.reset();
	return ptr;
}

template <class T>
std::shared_ptr<T> GameResourcesLoader::Create(const std::filesystem::path &aFilePath)
{
	OPTICK_EVENT();
	const std::type_info *typeInfo = &typeid(T);
	std::shared_ptr<Library> library = GetLibraryOfType<T>();

	if (!library)
	{
		library = std::make_shared<Library>();
		myLibraries[typeInfo] = library;
	}

	std::filesystem::path Identifier = aFilePath;
	std::filesystem::path loadObjectFrom = workingDirectory / aFilePath;

	auto ptr = (library->Get<T>(Identifier));

	if (!ptr || !ptr->isLoadedComplete)
	{
		std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
		ptr = library->Add(newObject);
		newObject.second->Init();
	}

	return ptr; 
}
