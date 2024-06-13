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
	bool HasComponent() const;

	template <class T>
	T* TryGetComponent();

	template <class T>
	T& GetComponent();

	std::vector<Component*> GetAllComponents() const;

	bool GetActive() const;
	void SetActive(bool aState) const;

	std::string GetName() const { return myManager->GetName(myID); };;
	void SetName(const std::string& name) const { myManager->SetName(name,myID); };

	void OnSiblingChanged(const std::type_info* SourceClass = nullptr) const;

	void SetLayer(const Layer aLayer) const { myManager->SetLayer(myID,aLayer); };
	Layer GetLayer() const { return myManager->GetLayer(myID); };

	SY::UUID GetID() const { return myID; }
	bool IsValid() const { return myID.IsValid(); }

private:
	friend class GameObjectManager; //Only the asset manager can create and destroy components 
	GameObject(const SY::UUID anID,GameObjectManager* aManager) : myID(anID),myManager(aManager) {}

	//DO NOT ADD VARIABLES HERE IT IS INCONSISTENT WITH PHILOSOPHY AND WILL NOT WORK
	SY::UUID myID = UINT_MAX;
	GameObjectManager* myManager = nullptr;
};

template<class T>
T& GameObject::AddComponent()
{
	return myManager->AddComponent<T>(myID);
}

template<class T>
T& GameObject::AddComponent(const T& aComponent)
{
	return myManager->AddComponent<T>(myID,aComponent);
}

template<class T,typename ...Args>
T& GameObject::AddComponent(Args ...someParameters)
{
	return myManager->AddComponent<T>(myID,someParameters...);
}

template<class T>
bool GameObject::HasComponent() const
{
	return myManager->HasComponent<T>(myID);
}

template<class T>
T* GameObject::TryGetComponent()
{
	return myManager ? myManager->TryGetComponent<T>(myID) : nullptr;
}

template<class T>
T& GameObject::GetComponent()
{
	return myManager->GetComponent<T>(myID);
}

inline std::vector<Component*> GameObject::GetAllComponents() const
{
	return myManager->GetAllAttachedComponents(myID);
}

inline bool GameObject::GetActive() const
{
	return myManager->GetActive(myID);
}

inline void GameObject::SetActive(const bool aState) const
{
	myManager->SetActive(myID,aState);
}

inline void GameObject::OnSiblingChanged(const std::type_info* SourceClass) const
{
	myManager->OnSiblingChanged(myID,SourceClass);
}

