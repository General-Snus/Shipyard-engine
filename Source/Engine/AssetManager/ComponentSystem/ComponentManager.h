
#ifndef ComponentManagerDef
#define ComponentManagerDef
#include "UUID.h"
#include <Tools/Reflection/refl.hpp>
#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

class GameObjectManager;
class Component;

class ComponentManagerBase
{
  public:
    friend class GameObjectManager;
    enum class UpdatePriority
    {
        Transform,
        Normal = 100,
        Physics,
        Collision,
        Render
    };

    ComponentManagerBase(GameObjectManager *manager, std::string typeName);
    virtual ~ComponentManagerBase() = default;
    virtual void Destroy() = 0;
    virtual std::shared_ptr<ComponentManagerBase> Clone() const = 0;

    virtual Component *DeepCopy(const Component *cmp) = 0;
    virtual Component *DeepCopy(const SY::UUID aGameObjectID) = 0;
    virtual void Merge(const ComponentManagerBase *other, SY::UUID gameobjectRebaseIndex) = 0;
    virtual void RebaseSelf(SY::UUID gameobjectRebaseIndex, GameObjectManager *newManager) = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void DeleteGameObject(const SY::UUID aGameObjectID) = 0;
    virtual void OnColliderEnter(const SY::UUID aFirstID, const SY::UUID aTargetID) = 0;
    virtual void OnColliderExit(const SY::UUID aFirstID, const SY::UUID aTargetID) = 0;
    virtual void OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info *SourceClass = nullptr) = 0;
    // Bad, Not knowing type forces dynamic cast for type
    virtual Component *AddComponent(SY::UUID aGameObjectID, const Component *aComponent) = 0;
    virtual bool ValidComponentType(const Component *cmp);
    void SetUpdatePriority(const UpdatePriority aPriority)
    {
        myUpdatePriority = aPriority;
    }
    UpdatePriority GetUpdatePriority() const
    {
        return myUpdatePriority;
    }

    // Cost: 1 Contains,1 Get, 1 reintp cast
    virtual Component *TryGetBaseComponent(const SY::UUID aGameObjectID) = 0;

    std::string GetType() const
    {
        return Comparator;
    }

  protected:
    std::string Comparator;
    GameObjectManager *myManager = nullptr;
    std::unordered_map<SY::UUID, unsigned int> myGameObjectIDtoVectorIndex;
    std::unordered_map<unsigned int, SY::UUID> myVectorIndexToGameObjectID;

  private:
    UpdatePriority myUpdatePriority = ComponentManagerBase::UpdatePriority::Normal;
};

template <class T> class ComponentManager : public ComponentManagerBase
{
  public:
    ComponentManager(GameObjectManager *manager) : ComponentManagerBase(manager, refl::reflect<T>().name.str()) {};
    ~ComponentManager() = default;
    void Destroy() override;

    std::shared_ptr<ComponentManagerBase> Clone() const override
    {
        return std::make_shared<ComponentManager<T>>(*this);
    }

    void Merge(const ComponentManagerBase *other, SY::UUID gameobjectRebaseIndex) override;
    void RebaseSelf(SY::UUID gameobjectRebaseIndex, GameObjectManager *newManager) override;
    Component *DeepCopy(const Component *cmp) override;
    Component *DeepCopy(const SY::UUID aGameObjectID) override;

    // Bad, Not knowing type forces dynamic cast for type checking
    Component *AddComponent(SY::UUID aGameObjectID, const Component *aComponent) override;
    T &AddComponent(const SY::UUID aGameObjectID);
    T &AddComponent(const SY::UUID aGameObjectID, const T &aComponent);
    template <typename... Args> T &AddComponent(const SY::UUID aGameObjectID, Args... someParameters);

    const bool HasComponent(const SY::UUID aGameObjectID) const;

    std::vector<T> &GetAllComponents();
    Component *TryGetBaseComponent(const SY::UUID aGameObjectID) override;
    T &GetComponent(const SY::UUID aGameObjectID);
    T *TryGetComponent(const SY::UUID aGameObjectID);

    void Update() override;
    void Render() override;
    void DeleteGameObject(const SY::UUID aGameObjectID) override;
    void OnColliderEnter(const SY::UUID aFirstID, const SY::UUID aTargetID) override;
    void OnColliderExit(const SY::UUID aFirstID, const SY::UUID aTargetID) override;
    void OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info *SourceClass = nullptr) override;

  private:
    std::vector<T> myComponents;
};

template <class T> inline void ComponentManager<T>::Destroy()
{
    myGameObjectIDtoVectorIndex.clear();
    myVectorIndexToGameObjectID.clear();
    myComponents.clear();
}

template <class T> void ComponentManager<T>::Merge(const ComponentManagerBase *other, SY::UUID gameobjectRebaseIndex)
{
    const ComponentManager<T> *copieSource = dynamic_cast<const ComponentManager<T> *>(other);

    const auto startEndpoint = (unsigned)myComponents.size();
    myComponents.reserve(startEndpoint + (unsigned)copieSource->myComponents.size());

    for (const T &i : copieSource->myComponents)
    {
        myComponents.emplace_back((i));
        myComponents.back().Rebase(gameobjectRebaseIndex, myManager);
    }

    for (auto &[id, vectorIndex] : copieSource->myGameObjectIDtoVectorIndex)
    {
        myGameObjectIDtoVectorIndex.emplace(id + gameobjectRebaseIndex, vectorIndex + startEndpoint);
        myVectorIndexToGameObjectID.emplace(vectorIndex + startEndpoint, id + gameobjectRebaseIndex);
    }
}

template <class T> void ComponentManager<T>::RebaseSelf(SY::UUID gameobjectRebaseIndex, GameObjectManager *newManager)
{
    myManager = newManager;

    for (T &i : myComponents)
    {
        i.Rebase(gameobjectRebaseIndex, myManager);
    }

    std::unordered_map<SY::UUID, unsigned int> newIDToVector;

    for (auto &[id, vectorIndex] : myGameObjectIDtoVectorIndex)
    {
        newIDToVector.emplace(id + gameobjectRebaseIndex, vectorIndex);
        myVectorIndexToGameObjectID.at(vectorIndex) = id + gameobjectRebaseIndex;
    }

    myGameObjectIDtoVectorIndex.clear();
    myGameObjectIDtoVectorIndex = newIDToVector;
}

template <class T> Component *ComponentManager<T>::TryGetBaseComponent(const SY::UUID aGameObjectID)
{
    return reinterpret_cast<Component *>(TryGetComponent(aGameObjectID));
}

template <class T> T &ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID)
{
    if (HasComponent(aGameObjectID))
    {
        return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
    }

    myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
    myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
    myComponents.push_back(T(aGameObjectID, myManager));
    static_cast<Component *>(&myComponents.back())->SetManager(myManager);
    static_cast<Component *>(&myComponents.back())->Init();
    return myComponents.back();
}

template <class T>
inline Component *ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, const Component *aComponent)
{
    const T *cmp = dynamic_cast<const T *>(aComponent);
    if (cmp != nullptr)
    {
        return &AddComponent(aGameObjectID, *cmp);
    }
    return nullptr;
}

template <class T> inline T &ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, const T &aComponent)
{
    if (HasComponent(aGameObjectID))
    {
        return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
    }

    myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
    myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
    myComponents.push_back(aComponent);
    static_cast<Component *>(&myComponents.back())->SetOwnerID(aGameObjectID);
    static_cast<Component *>(&myComponents.back())->SetManager(myManager);
    static_cast<Component *>(&myComponents.back())->Init();
    return myComponents.back();
}

template <class T>
template <typename... Args>
inline T &ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, Args... someParameters)
{
    if (HasComponent(aGameObjectID))
    {
        return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
    }

    myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
    myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
    myComponents.push_back(T(aGameObjectID, myManager, someParameters...));
    static_cast<Component *>(&myComponents.back())->SetManager(myManager);
    static_cast<Component *>(&myComponents.back())->Init();
    return myComponents.back();
}

template <class T> inline const bool ComponentManager<T>::HasComponent(const SY::UUID aGameObjectID) const
{
    return myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end();
}

template <class T> inline std::vector<T> &ComponentManager<T>::GetAllComponents()
{
    return myComponents;
}

template <class T> inline T &ComponentManager<T>::GetComponent(const SY::UUID aGameObjectID)
{
    assert(myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end() &&
           "ComponentManager::GetComponent(...) tried to get a missing "
           "component. Maybe it's best if you use "
           "TryGetComponent instead.");
    return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
}

template <class T> T *ComponentManager<T>::TryGetComponent(const SY::UUID aGameObjectID)
{
    if (myGameObjectIDtoVectorIndex.contains(aGameObjectID))
    {
        return &myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
    }
    return nullptr;
}

template <class T> Component *ComponentManager<T>::DeepCopy(const Component *cmp)
{
    const T *cmpT = dynamic_cast<const T *>(cmp);
    if (cmpT != nullptr)
    {
        return new T(*cmpT);
    }
    return nullptr;
}

template <class T> Component *ComponentManager<T>::DeepCopy(const SY::UUID aGameObjectID)
{
    if (myGameObjectIDtoVectorIndex.contains(aGameObjectID))
    {
        const T *cmpT = dynamic_cast<const T *>(&myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]]);
        if (cmpT != nullptr)
        {
            return new T(*cmpT);
        }
    }
    return nullptr;
}

template <class T> void ComponentManager<T>::Update()
{
    for (size_t i = 0; i < myComponents.size(); i++)
    {
        if (myComponents[i].IsActive() && !myComponents[i].IsAdopted())
        {
            myComponents[i].Update();
        }
    }
}

template <class T> void ComponentManager<T>::Render()
{
    for (size_t i = 0; i < myComponents.size(); i++)
    {
        if (myComponents[i].IsActive())
        {
            myComponents[i].Render();
        }
    }
}

template <class T> void ComponentManager<T>::DeleteGameObject(const SY::UUID aGameObjectID)
{
    const auto it = myGameObjectIDtoVectorIndex.find(aGameObjectID);
    if (it == myGameObjectIDtoVectorIndex.end())
    {
        return;
    }
    const unsigned int index = it->second;
    const unsigned int id = myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size() - 1)];
    myComponents[index].Destroy();
    myComponents[index] = myComponents[myComponents.size() - 1];
    // std::swap(myComponents[index],myComponents[myComponents.size() - 1]);
    myComponents.pop_back();

    myGameObjectIDtoVectorIndex[id] = index;
    myVectorIndexToGameObjectID[index] = id;

    myVectorIndexToGameObjectID.erase(myVectorIndexToGameObjectID.at(static_cast<unsigned int>(myComponents.size())));
    myGameObjectIDtoVectorIndex.erase(it->first);
}

template <class T> void ComponentManager<T>::OnColliderEnter(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
    if (myGameObjectIDtoVectorIndex.find(aFirstID) != myGameObjectIDtoVectorIndex.end())
    {
        const auto id = myGameObjectIDtoVectorIndex[aFirstID];
        if (myComponents[id].IsActive())
        {
            myComponents[id].OnColliderEnter(aTargetID);
        }
    }

    if (myGameObjectIDtoVectorIndex.find(aTargetID) != myGameObjectIDtoVectorIndex.end())
    {
        const auto id = myGameObjectIDtoVectorIndex[aTargetID];
        if (myComponents[id].IsActive())
        {
            myComponents[id].OnColliderEnter(aFirstID);
        }
    }
}

template <class T> void ComponentManager<T>::OnColliderExit(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
    if (myGameObjectIDtoVectorIndex.find(aFirstID) != myGameObjectIDtoVectorIndex.end())
    {
        const auto id = myGameObjectIDtoVectorIndex[aFirstID];
        if (myComponents[id].IsActive())
        {
            myComponents[id].OnColliderExit(aTargetID);
        }
    }

    if (myGameObjectIDtoVectorIndex.find(aTargetID) != myGameObjectIDtoVectorIndex.end())
    {
        const auto id = myGameObjectIDtoVectorIndex[aTargetID];
        if (myComponents[id].IsActive())
        {
            myComponents[id].OnColliderExit(aFirstID);
        }
    }
}

template <class T>
inline void ComponentManager<T>::OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info *SourceClass)
{
    if (SourceClass == &typeid(T))
    {
        return;
    }

    if (myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end())
    {
        myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]].OnSiblingChanged(SourceClass);
    }
}

#endif
