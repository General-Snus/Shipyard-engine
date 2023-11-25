#pragma once
#include "GameObject.h"
#include "GameObjectManager.h"

enum class eComponentType
{
	base,
	backgroundColor,
};

class Component
{
public:
	Component(const SY::UUID anOwnerID) : myOwnerID(anOwnerID),myIsActive(true),myComponentType(eComponentType::base) {}
	Component(const SY::UUID anOwnerID,eComponentType aComponentType) : myOwnerID(anOwnerID),myIsActive(true),myComponentType(aComponentType) {}
	virtual ~Component() = default;

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}

	template <class T>
	bool HasComponent() const;

	inline SY::UUID GetOwner() { return myOwnerID; }
	inline GameObject GetGameObject() { return GameObjectManager::GetInstance().GetGameObject(myOwnerID); }

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


	inline bool IsActive() { return myIsActive && GameObjectManager::GetInstance().GetActive(myOwnerID); }
	inline void SetActive(const bool aState) { myIsActive = aState; }

	virtual void CollidedWith(const SY::UUID /*aGameObjectID*/) {}

	bool IsAdopted() { return IsInherited; };
	void Adopt() { IsInherited++; }
	void Abandon();
protected:
	SY::UUID myOwnerID;
	eComponentType myComponentType;

	//IsInherited is a new system that allows a component to remove an other component from the update loop and promise to take care of it themselves
	int IsInherited = 0;

private:
	Component() = default;

	template <class T>
	friend class ComponentManager;

	bool myIsActive = true;

	inline void SetOwnerID(const SY::UUID anOwnerID)
	{
		myOwnerID = anOwnerID;
	}
};

template <class T>
bool Component::HasComponent() const
{
	return GameObjectManager::GetInstance().HasComponent<T>(myOwnerID);
}

template<class T>
inline T& Component::GetComponent()
{
	return GameObjectManager::GetInstance().GetComponent<T>(myOwnerID);
}

template<class T>
inline T* Component::TryGetComponent()
{
	return GameObjectManager::GetInstance().TryGetComponent<T>(myOwnerID);
}

template<class T>
inline T* Component::TryGetAddComponent()
{
	if(auto* returnComponent = GameObjectManager::GetInstance().TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return &GameObjectManager::GetInstance().AddComponent<T>(myOwnerID);
}


template<class T>
inline const T& Component::GetComponent() const 
{
	return GameObjectManager::GetInstance().GetComponent<T>(myOwnerID);
}

template<class T>
inline const T* Component::TryGetComponent() const 
{
	return GameObjectManager::GetInstance().TryGetComponent<T>(myOwnerID);
}

template<class T>
inline const T* Component::TryGetAddComponent() const 
{
	if(auto* returnComponent = GameObjectManager::GetInstance().TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return GameObjectManager::GetInstance().AddComponent<T>(myOwnerID);
}

