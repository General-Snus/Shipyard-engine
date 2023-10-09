#pragma once
#include "../GameObjects/GameObject.h"
#include <AssetManager/GameObjectManager.h>

enum class eComponentType
{
	base,
	backgroundColor,
};

class Component
{
public:
	Component() = default;
	Component(const unsigned int anOwnerID) : myOwnerID(anOwnerID),myIsActive(true),myComponentType(eComponentType::base) {}
	Component(const unsigned int anOwnerID,eComponentType aComponentType) : myOwnerID(anOwnerID),myIsActive(true),myComponentType(aComponentType) {}
	virtual ~Component() = default;

	virtual void Init() {} 
	virtual void Update() {}
	virtual void Render() {} 

	template <class T>
	const bool HasComponent() const;

	inline unsigned int GetOwner() { return myOwnerID; }
	inline GameObject GetGameObject() { return GameObjectManager::GetInstance().GetGameObject(myOwnerID); }

	template <class T>
	T& GetComponent();

	template <class T>
	T* TryGetComponent();

	inline bool IsActive() { return myIsActive && GameObjectManager::GetInstance().GetActive(myOwnerID); }
	inline void SetActive(const bool aState) { myIsActive = aState; }

	virtual void CollidedWith(const unsigned int /*aGameObjectID*/) {}

protected:
	unsigned int myOwnerID;
	eComponentType myComponentType;

private:
	template <class T>
	friend class ComponentManager;

	bool myIsActive = true;

	inline void SetOwnerID(const unsigned int anOwnerID)
	{
		myOwnerID = anOwnerID;
	}
};

template <class T>
const bool Component::HasComponent() const
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
