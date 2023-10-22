#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include "Objects/Components/ComponentManager.h"
 
class GameObject;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager();

	static GameObjectManager& GetInstance();

	const GameObject CreateGameObject();
	void DeleteGameObject(const unsigned int aGameObjectID);
	void DeleteGameObject(const GameObject aGameObject);

	template <class T>
	T& AddComponent(const unsigned int aGameObjectID);

	template <class T>
	T& AddComponent(const unsigned int aGameObjectID,const T& aComponent);

	template <class T,typename... Args>
	T& AddComponent(const unsigned int aGameObjectID,Args... someParameters);

	template<class T>
	std::vector<T>& GetAllComponents();

	template <class T>
	T* TryGetComponent(const unsigned int aGameObjectID);

	template <class T>
	const bool HasComponent(const unsigned int aGameObjectID);

	template <class T>
	T& GetComponent(const unsigned int aGameObjectID);

	bool GetActive(const unsigned int aGameObjectID);

	GameObject GetWorldRoot();
	GameObject GetPlayer();
	GameObject GetCamera(); 
	GameObject GetGameObject(unsigned int anID);

	void CollidedWith(const unsigned int aFirstID,const unsigned int aTargetID); 
	void SetActive(const unsigned int aGameObjectID,const bool aState);

	void SetLastGOAsPlayer();
	void SetLastGOAsWorld(); 
	void SetLastGOAsCamera();

	template <class T>
	void SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority);

	void Update();


private:
	template <class T>
	void AddManager();
	void SortUpdateOrder();
	void DeleteObjects();

	std::unordered_map<const std::type_info*,ComponentManagerBase*> myComponentManagers = { };
	std::unordered_map<unsigned int,bool> myGameObjects = { };
	std::vector<std::pair<const std::type_info*,ComponentManagerBase*>> myUpdateOrder = { };
	std::vector<unsigned int> myObjectsToDelete = { };
	unsigned int myLastID = 0;
	unsigned int myWorldRoot;
	unsigned int myPlayer;
	unsigned int myCamera;
};

template<class T>
T& GameObjectManager::AddComponent(const unsigned int aGameObjectID)
{
	if(!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID);

}

template<class T>
T& GameObjectManager::AddComponent(const unsigned int aGameObjectID,const T& aComponent)
{
	if(!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID,aComponent);
}

template<class T,typename... Args>
T& GameObjectManager::AddComponent(const unsigned int aGameObjectID,Args ...someParameters)
{
	if(!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID,someParameters...);
}

template<class T>
std::vector<T>& GameObjectManager::GetAllComponents()
{
	if(!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->GetAllComponents();
}

template<class T>
T* GameObjectManager::TryGetComponent(const unsigned int aGameObjectID)
{
	if(myComponentManagers.contains(&typeid(T)))
	{
		return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->TryGetComponent(aGameObjectID);
	}
	std::cout << "GameObjectManager: Tried to get a component which was not present on GameObjectID : " << aGameObjectID << std::endl;
	//assert(false && "GameObjectManager::TryGetComponent(...) component manager doesn't exist.");
	//ERROR_PRINT("GameObjectManager: Tried to get a component which was not present on GameObjectID: " + aGameObjectID);
	return nullptr;
}

template<class T>
inline const bool GameObjectManager::HasComponent(const unsigned int aGameObjectID)
{
	if(myComponentManagers.contains(&typeid(T)))
	{
		return static_cast<const ComponentManager<T>*>(myComponentManagers[&typeid(T)])->HasComponent(aGameObjectID);
	}
	return false;
}

template<class T>
inline T& GameObjectManager::GetComponent(const unsigned int aGameObjectID)
{
	assert(myComponentManagers.contains(&typeid(T)) && "GameObjectManager::GetComponent(...) component manager doesn't exist.");
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->GetComponent(aGameObjectID);
}

template<class T>
void GameObjectManager::SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority)
{
	if(!myComponentManagers.contains(&typeid(T)))
	{
		myComponentManagers[&typeid(T)] = new ComponentManager<T>();
	}
	myComponentManagers[&typeid(T)]->SetUpdatePriority(aPriority);
	SortUpdateOrder();
}

template<class T>
void GameObjectManager::AddManager()
{
	myComponentManagers[&typeid(T)] = new ComponentManager<T>();
	SortUpdateOrder();
}