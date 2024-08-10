#pragma once
#define AsUINT(v) static_cast<unsigned>(v)

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
    friend class AssetManager;

  public:
    Library() = default;
    template <class T> std::shared_ptr<T> Add(const std::pair<std::filesystem::path, std::shared_ptr<T>> &pair);

    template <class T> std::shared_ptr<T> Get(const std::filesystem::path &aFilePath);

    template <class T> std::unordered_map<std::filesystem::path, std::shared_ptr<T>> GetContentCatalogue();

  private:
    assetMap content;
};

class AssetManager : public Singleton<AssetManager>
{
    friend class Singleton<AssetManager>;

  public:
    ~AssetManager() = default;

    template <class T> std::shared_ptr<T> LoadAsset(const std::filesystem::path &aFilePath);
    template <class T> std::shared_ptr<T> LoadAsset(const std::filesystem::path &aFilePath, bool useExact);

    template <class T> void HasAsset(const std::filesystem::path &aFilePath, bool useExact) const;
    template <class T> bool ForceLoadAsset(const std::filesystem::path &aFilePath, std::shared_ptr<T> &outAsset);
    template <class T>
    bool ForceLoadAsset(const std::filesystem::path &aFilePath, bool useExact, std::shared_ptr<T> &outAsset);

    void SubscribeToChanges(const std::filesystem::path &aFilePath, SY::UUID gameobjectID);

    bool AdaptPath(std::filesystem::path &path);

    // TODO just because this isnt optimal
    // Shame place, these are needed for the hopefully nieche condition of not knowing what you are loading
    static std::string AssetType(const std::filesystem::path &path);
    static std::shared_ptr<AssetBase> TryLoadAsset(const std::filesystem::path &path);

    static inline constexpr char exprAssetPath[15] = "../../Content";
    static inline std::filesystem::path AssetPath = exprAssetPath;

    // CAREFUL YOU FOOL
    template <class T> std::shared_ptr<Library> GetLibraryOfType();

  private:
    AssetManager();
    // thread
    void ThreadedLoading();
    Queue<std::shared_ptr<AssetBase>> myAssetQueue;
    // NameToPath
    void RecursiveNameSave();
    std::unordered_map<std::filesystem::path, std::filesystem::path> nameToPathMap;

    // Libraries
    std::unordered_map<const std::type_info *, std::shared_ptr<Library>> myLibraries;
};

template <class T>
bool AssetManager::ForceLoadAsset(const std::filesystem::path &aFilePath, std::shared_ptr<T> &outAsset)
{
    return ForceLoadAsset<T>(aFilePath, false, outAsset);
}

template <class T>
bool AssetManager::ForceLoadAsset(const std::filesystem::path &identifierPath, const bool useExact,
                                  std::shared_ptr<T> &outAsset)
{
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
        Identifier = std::filesystem::relative(
            identifierPath, AssetPath); // Might invoke unexpected behaviour but stops you from duplication

        // TODO FIgure out if use exact is even a good thing to have? Can i automate without any sliver of doubt?
    }
    else
    {
        loadObjectFrom = AssetPath / loadObjectFrom;
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

// Runs function F, get asset send function f asset
inline void AssetManager::SubscribeToChanges(const std::filesystem::path &aFilePath, const SY::UUID gameobjectID)
{
    aFilePath;
    gameobjectID;
    // AssetCallbackMaster::dataStruct arg;
    // arg.subscribed = gameobjectID;
    //
    // assetCallbackMaster.callbacks.try_emplace(aFilePath,arg);
}

/// <summary>
/// Holds the current thread until the asset is loaded
/// </summary>
template <class T> std::shared_ptr<T> AssetManager::LoadAsset(const std::filesystem::path &aFilePath)
{
    return LoadAsset<T>(aFilePath, false);
}

/// <summary>
/// Holds the current thread until the asset is loaded
/// </summary>
template <class T>
std::shared_ptr<T> AssetManager::LoadAsset(const std::filesystem::path &identifierPath, const bool useExact)
{
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
        Identifier = std::filesystem::relative(identifierPath, AssetPath);
    }
    else
    {
        loadObjectFrom = AssetPath / loadObjectFrom;
    }

    std::shared_ptr<T> ptr = library->Get<T>(Identifier);

    if (!ptr)
    {
        std::pair<std::filesystem::path, std::shared_ptr<T>> newObject(Identifier, std::make_shared<T>(loadObjectFrom));
        ptr = library->Add(newObject);
        myAssetQueue.EnqueueUnique(newObject.second);
        newObject.second->isBeingLoaded = true;
#if ThreadedAssetLoading
        ThreadPool::Get().SubmitWork(std::bind(&AssetManager::ThreadedLoading, this));
#else
        ThreadedLoading();
#endif
    }
    return ptr;
}

template <class T> void AssetManager::HasAsset(const std::filesystem::path &aFilePath, const bool useExact) const
{
    aFilePath;
    useExact;
    // AMLogger.Err("NotImplementedException");
    assert(false);
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

template <class T> std::unordered_map<std::filesystem::path, std::shared_ptr<T>> Library::GetContentCatalogue()
{
    std::unordered_map<std::filesystem::path, std::shared_ptr<T>> newOutMap;
    for (auto &i : content)
    {
        newOutMap.emplace(i.first, std::dynamic_pointer_cast<T>(i.second));
    }
    return newOutMap;
}

template <class T> std::shared_ptr<Library> AssetManager::GetLibraryOfType()
{
    const std::type_info *typeInfo = &typeid(T);
    auto it = myLibraries.find(typeInfo);
    if (it != myLibraries.end())
    {
        return it->second;
    }
    return nullptr;
}
