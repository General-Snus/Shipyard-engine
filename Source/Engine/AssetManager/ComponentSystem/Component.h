#pragma once
#include <memory>
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "Tools/Reflection/refl.hpp"

enum class eComponentType
{
	base,
	backgroundColor,
};

class Component : public Reflectable
{
public:
	MYLIB_REFLECTABLE();
	 
	Component(const SY::UUID anOwnerID,GameObjectManager* aManager) : myOwnerID(anOwnerID),myManager(aManager),m_IsActive(true),myComponentType(eComponentType::base) {}
	Component(const SY::UUID anOwnerID,GameObjectManager* aManager,eComponentType aComponentType) : myOwnerID(anOwnerID),myManager(aManager),m_IsActive(true),myComponentType(aComponentType) {}
	virtual ~Component() noexcept = default;

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void Destroy() {}
	virtual void OnSiblingChanged(const std::type_info* SourceClass = nullptr) { SourceClass; };

	template <class T>
	bool HasComponent() const;

	inline const SY::UUID GetOwner()  const { return myOwnerID; }
	inline GameObject GetGameObject() const { return myManager->GetGameObject(myOwnerID); }

	template <class T>
	T& GetComponent();
	template <class T>
	T* TryGetComponent();
	template <class T>
	T* TryGetAddComponent();

	template<class T>
	const T& GetComponent() const;
	template <class T>
	const T* TryGetComponent() const;
	template <class T>
	const T* TryGetAddComponent() const;
	void InspectorView() override;

	inline bool IsActive() const { return m_IsActive && myManager->GetActive(myOwnerID); }
	inline void SetActive(const bool aState) { m_IsActive = aState; }

	virtual void CollidedWith(const SY::UUID /*aGameObjectID*/) {}

	bool IsAdopted() { return IsInherited; };
	void Adopt() { IsInherited++; }
	void Abandon();



	SY::UUID myOwnerID;
	GameObjectManager* myManager = nullptr;
	eComponentType myComponentType;

	//IsInherited is a new system that allows a component to remove an other component from the update loop and promise to take care of it themselves
	int IsInherited = 0;
	bool m_IsActive = true;
private:
	Component() = default;

	template <class T>
	friend class ComponentManager;


	inline void SetOwnerID(const SY::UUID anOwnerID)
	{
		myOwnerID = anOwnerID;
	}

	inline void SetManager(GameObjectManager* aManager)
	{
		myManager = aManager;
	}
};


REFL_AUTO(type(Component)
	,field(myOwnerID)
	,field(m_IsActive)
)





template <class T>
bool Component::HasComponent() const
{
	return GameObjectManager::Get().HasComponent<T>(myOwnerID);
}

template<class T>
inline T& Component::GetComponent()
{
	return GameObjectManager::Get().GetComponent<T>(myOwnerID);
}

template<class T>
inline T* Component::TryGetComponent()
{
	return GameObjectManager::Get().TryGetComponent<T>(myOwnerID);
}

template<class T>
inline T* Component::TryGetAddComponent()
{
	if (auto* returnComponent = GameObjectManager::Get().TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return &GameObjectManager::Get().AddComponent<T>(myOwnerID);
}


template<class T>
inline const T& Component::GetComponent() const
{
	return GameObjectManager::Get().GetComponent<T>(myOwnerID);
}

template<class T>
inline const T* Component::TryGetComponent() const
{
	return GameObjectManager::Get().TryGetComponent<T>(myOwnerID);
}

template<class T>
inline const T* Component::TryGetAddComponent() const
{
	if (auto* returnComponent = GameObjectManager::Get().TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return GameObjectManager::Get().AddComponent<T>(myOwnerID);
}

