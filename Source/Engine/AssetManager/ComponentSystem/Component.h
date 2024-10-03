#pragma once

#ifndef ComponentDef
#define ComponentDef
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h"
#include "GameObject.h"
#include "GameObjectManager.h"

enum class eComponentType
{
    base,
    backgroundColor,
};

class Transform;
class Component : public Reflectable
{
  public:
    MYLIB_REFLECTABLE();
    Component(const SY::UUID anOwnerID, GameObjectManager *aManager)
        : myOwnerID(anOwnerID), myManager(aManager), m_IsActive(true), myComponentType(eComponentType::base)
    {
    }

    Component(const SY::UUID anOwnerID, GameObjectManager *aManager, eComponentType aComponentType)
        : myOwnerID(anOwnerID), myManager(aManager), m_IsActive(true), myComponentType(aComponentType)
    {
    }

    Component(const Component &aComponent) = default;
    virtual ~Component() noexcept = default;

    virtual void Init()
    {
    }
    virtual void Update()
    {
    }
    virtual void Render()
    {
    }
    virtual void Destroy()
    {
    }
    virtual void OnSiblingChanged(const std::type_info *SourceClass = nullptr)
    {
        SourceClass;
    };

    template <class T> bool HasComponent() const;

    const SY::UUID GetOwner() const
    {
        return myOwnerID;
    }
    GameObject GetGameObject() const
    {
        return myManager->GetGameObject(myOwnerID);
    }

    template <class T> T &GetComponent();
    template <class T> T *TryGetComponent();
    template <class T> T *TryGetAddComponent();

    template <class T> const T &GetComponent() const;
    template <class T> const T *TryGetComponent() const;
    template <class T> const T *TryGetAddComponent() const;
    bool InspectorView() override;

    bool IsActive() const
    {
        return m_IsActive && myManager->GetActive(myOwnerID);
    }
    void SetActive(const bool aState)
    {
        m_IsActive = aState;
    }

    virtual void OnColliderEnter(const SY::UUID /*aGameObjectID*/)
    {
    }

    virtual void OnColliderExit(const SY::UUID /*aGameObjectID*/)
    {
    }

    bool IsAdopted()
    {
        return IsInherited;
    };
    void Adopt()
    {
        IsInherited++;
    }
    void Abandon();
    Transform &transform();

    SY::UUID myOwnerID;
    GameObjectManager *myManager = nullptr;
    eComponentType myComponentType;

    // IsInherited is a new system that allows a component to remove an other component from the update loop and promise
    // to take care of it themselves
    int IsInherited = 0;
    bool m_IsActive = true;

    virtual void Rebase(const SY::UUID newBase, GameObjectManager *aManager)
    {
        SetOwnerID(myOwnerID + newBase);
        SetManager(aManager);
    }

  private:
    Component() = default;

    template <class T> friend class ComponentManager;
    void SetOwnerID(const SY::UUID anOwnerID)
    {
        myOwnerID = anOwnerID;
    }

    void SetManager(GameObjectManager *aManager)
    {
        myManager = aManager;
    }
};

REFL_AUTO(type(Component), field(myOwnerID), field(m_IsActive))

template <class T> bool Component::HasComponent() const
{
    return myManager->HasComponent<T>(myOwnerID);
}

template <class T> T &Component::GetComponent()
{
    return myManager->GetComponent<T>(myOwnerID);
}

template <class T> T *Component::TryGetComponent()
{
    return myManager->TryGetComponent<T>(myOwnerID);
}

template <class T> T *Component::TryGetAddComponent()
{
    if (auto *returnComponent = myManager->TryGetComponent<T>(myOwnerID))
    {
        return returnComponent;
    }
    return &myManager->AddComponent<T>(myOwnerID);
}

template <class T> const T &Component::GetComponent() const
{
    return myManager->GetComponent<T>(myOwnerID);
}

template <class T> const T *Component::TryGetComponent() const
{
    return myManager->TryGetComponent<T>(myOwnerID);
}

template <class T> const T *Component::TryGetAddComponent() const
{
    if (auto *returnComponent = myManager->TryGetComponent<T>(myOwnerID))
    {
        return returnComponent;
    }
    return myManager->AddComponent<T>(myOwnerID);
}

#endif
