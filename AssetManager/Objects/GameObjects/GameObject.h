#pragma once

#include "../../GameObjectManager.h"
#define AsUINT(v) static_cast<unsigned>(v) 
 

class GameObject
{
public:
	GameObject() = default; // It's dangerous to create a gameobject without the manager!
	~GameObject() = default;

	template <class T>
	void AddComponent();

	template <class T>
	void AddComponent(const T& aComponent);

	template <class T,typename... Args>
	void AddComponent(Args... someParameters);

	template <class T>
	const bool HasComponent();

	template <class T>
	T* TryGetComponent();

	template <class T>
	T& GetComponent();

	bool GetActive();

	void SetActive(const bool aState);

	inline const unsigned int GetID() const { return myID; }

	const bool IsValid() const { return myID != UINT_MAX; }

private:
	friend class GameObjectManager; //Only the asset manager can create and destroy components

	GameObject(const unsigned int anID,GameObjectManager* aManager) : myID(anID),myManager(aManager) {}
	unsigned int myID = UINT_MAX;
	GameObjectManager* myManager = nullptr;
};


template<class T>
inline void GameObject::AddComponent()
{
	myManager->AddComponent<T>(myID);
}

template<class T>
inline void GameObject::AddComponent(const T& aComponent)
{
	myManager->AddComponent<T>(myID,aComponent);
}

template<class T,typename ...Args>
inline void GameObject::AddComponent(Args ...someParameters)
{
	myManager->AddComponent<T>(myID,someParameters...);
}

template<class T>
inline const bool GameObject::HasComponent()
{
	return myManager->HasComponent<T>(myID);
}

template<class T>
inline T* GameObject::TryGetComponent()
{
	return myManager->TryGetComponent<T>(myID);
}

template<class T>
inline T& GameObject::GetComponent()
{
	return myManager->GetComponent<T>(myID);
}

inline bool GameObject::GetActive()
{
	return myManager->GetActive(myID);
}

inline void GameObject::SetActive(const bool aState)
{
	myManager->SetActive(myID,aState);
}

