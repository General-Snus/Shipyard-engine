#pragma once
#define AsUINT(v) static_cast<unsigned>(v)

#include <Tools/Utilities/System/ServiceLocator.h>
#include <Editor/Editor/Defines.h>
#include <Engine/AssetManager/ComponentSystem/UUID.h>
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include <Tools/Utilities/DataStructures/Queue.hpp>
#include <Tools/Utilities/FileHelpers.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Tools/Utilities/System/ThreadPool.hpp>
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

#define EngineResources ServiceLocator::Instance().GetService<EngineResourcesLoader>()
#define Resources ServiceLocator::Instance().GetService<GameResourcesLoader>()

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
    auto it = myLibraries.find(typeInfo);
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
    std::shared_ptr<Library> library = GetLibraryOfType<T>();

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

class GameResourcesLoader : public ResourceLoaderBase
{
  public:
    template <class T> bool Find(const std::filesystem::path &aFilePath) const;
    template <class T> std::shared_ptr<T> Load(const std::filesystem::path &aFilePath);
    std::shared_ptr<AssetBase> TryLoad(const std::filesystem::path &path);
    template <class T> bool ForceLoad(const std::filesystem::path &aFilePath, std::shared_ptr<T> &outAsset);
};
