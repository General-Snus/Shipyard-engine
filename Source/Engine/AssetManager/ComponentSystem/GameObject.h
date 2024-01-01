#pragma once

#include "GameObjectManager.h"
#define AsUINT(v) static_cast<unsigned>(v)   


class GameObject
{
public:
	GameObject() = default; // It's dangerous to create a gameobject without the manager!
	~GameObject() = default;

	template <class T>
	T& AddComponent();

	template <class T>
	T& AddComponent(const T& aComponent);

	template <class T,typename... Args>
	T& AddComponent(Args... someParameters);

	template <class T>
	const bool HasComponent();

	template <class T>
	T* TryGetComponent();

	template <class T>
	T& GetComponent();

	bool GetActive();
	void SetActive(const bool aState);

	inline void SetLayer(const Layer aLayer) { myManager->SetLayer(myID,aLayer); };
	inline Layer GetLayer() const { return myManager->GetLayer(myID); };

	inline const SY::UUID GetID() const { return myID; }
	const bool IsValid() const { return myID.IsValid(); }

private:
	friend class GameObjectManager; //Only the asset manager can create and destroy components



	//DO NOT ADD VARIABLES HERE IT IS INCONSISTENT WITH PHILOSOPHY AND WILL NOT WORK
	GameObject(const SY::UUID anID,GameObjectManager* aManager) : myID(anID),myManager(aManager) {}
	SY::UUID myID = UINT_MAX;

	GameObjectManager* myManager = nullptr;
};

template<class T>
inline T& GameObject::AddComponent()
{
	return myManager->AddComponent<T>(myID);
}

template<class T>
inline T& GameObject::AddComponent(const T& aComponent)
{
	return myManager->AddComponent<T>(myID,aComponent);
}

template<class T,typename ...Args>
inline T& GameObject::AddComponent(Args ...someParameters)
{
	return myManager->AddComponent<T>(myID,someParameters...);
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

