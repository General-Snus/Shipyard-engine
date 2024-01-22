#pragma once
#include <iostream>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "ComponentManager.h"


enum class Layer
{
	None = 0,
	Default = 1,
	Enemy = 1 << 2,
	Player = 1 << 3,
	Entities = Enemy | Player,
	Projectile = 1 << 4,
	Environment = 1 << 5,
	Count = 1 << 6,
	AllLayers = INT32_MAX
};

template<class T> inline Layer operator~ (Layer a) { return (Layer)~(int)a; }
template<class T> inline Layer operator| (Layer a,Layer b) { return (Layer)((int)a | (int)b); }
template<class T> inline Layer operator& (Layer a,Layer b) { return (Layer)((int)a & (int)b); }
template<class T> inline Layer operator^ (Layer a,Layer b) { return (Layer)((int)a ^ (int)b); }
template<class T> inline Layer& operator|= (Layer a,Layer b) { return (Layer&)((int&)a |= (int)b); }
template<class T> inline Layer& operator&= (Layer a,Layer b) { return (Layer&)((int&)a &= (int)b); }
template<class T> inline Layer& operator^= (Layer a,Layer b) { return (Layer&)((int&)a ^= (int)b); }


class GameObject;

class GameObjectManager : public Singleton<GameObjectManager>
{
	friend class Singleton<GameObjectManager>;
public:
	GameObjectManager() = default;
	~GameObjectManager();
	const GameObject CreateGameObject();
	const GameObject CreateGameObjectAt(const SY::UUID aGameObjectID);
	void DeleteGameObject(const SY::UUID aGameObjectID);
	void DeleteGameObject(const GameObject aGameObject);

	template <class T>
	T& AddComponent(const SY::UUID aGameObjectID);

	template <class T>
	T& AddComponent(const SY::UUID aGameObjectID,const T& aComponent);

	template <class T,typename... Args>
	T& AddComponent(const SY::UUID aGameObjectID,Args... someParameters);

	template<class T>
	std::vector<T>& GetAllComponents();

	template <class T>
	T* TryGetComponent(const SY::UUID aGameObjectID);

	template <class T>
	const bool HasComponent(const SY::UUID aGameObjectID);

	template <class T>
	T& GetComponent(const SY::UUID aGameObjectID);

	bool GetActive(const SY::UUID aGameObjectID);
	Layer GetLayer(const SY::UUID aGameObjectID);

	GameObject GetWorldRoot();
	GameObject GetPlayer();
	GameObject GetCamera();
	GameObject GetGameObject(SY::UUID anID);

	void CollidedWith(const SY::UUID aFirstID,const SY::UUID aTargetID);
	void SetActive(const SY::UUID aGameObjectID,const bool aState);
	void SetLayer(const SY::UUID aGameObjectID,const Layer alayer);

	void SetLastGOAsPlayer();
	void SetLastGOAsWorld();
	void SetLastGOAsCamera();

	//hack used to add and remove same id in same frame
	void CustomOrderUpdate();

	template <class T>
	void SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority);
	void Update();


	//Please dont call this for an other object than your own
	void OnSiblingChanged(SY::UUID anID,const std::type_info* SourceClass = nullptr);


private:
	struct GameObjectData
	{
		bool IsActive = true;
		Layer onLayer = Layer::Default;
	};

	template <class T>
	void AddManager();
	void SortUpdateOrder();
	void DeleteObjects();
	void AddObjects();

	std::unordered_map<const std::type_info*,ComponentManagerBase*> myComponentManagers = { };
	std::unordered_map<SY::UUID,GameObjectData> myGameObjects = { };
	std::vector<std::pair<const std::type_info*,ComponentManagerBase*>> myUpdateOrder = { };
	std::vector<SY::UUID> myObjectsToDelete = { };
	std::vector<SY::UUID> myObjectsToAdd = { };
	unsigned int myLastID = 0;
	unsigned int myWorldRoot;
	unsigned int myPlayer;
	unsigned int myCamera;
};

template<class T>
T& GameObjectManager::AddComponent(const SY::UUID aGameObjectID)
{
	if (!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID);
}

template<class T>
T& GameObjectManager::AddComponent(const SY::UUID aGameObjectID,const T& aComponent)
{
	if (!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID,aComponent);
}

template<class T,typename... Args>
T& GameObjectManager::AddComponent(const SY::UUID aGameObjectID,Args ...someParameters)
{
	if (!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->AddComponent(aGameObjectID,someParameters...);
}

template<class T>
std::vector<T>& GameObjectManager::GetAllComponents()
{
	if (!myComponentManagers.contains(&typeid(T)))
	{
		AddManager<T>();
	}
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->GetAllComponents();
}

template<class T>
T* GameObjectManager::TryGetComponent(const SY::UUID aGameObjectID)
{
	if (myComponentManagers.contains(&typeid(T)))
	{
		return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->TryGetComponent(aGameObjectID);
	}
	std::cout << "GameObjectManager: Tried to get a component which was not present on GameObjectID : " << aGameObjectID << std::endl;
	//assert(false && "GameObjectManager::TryGetComponent(...) component manager doesn't exist.");
	//ERROR_PRINT("GameObjectManager: Tried to get a component which was not present on GameObjectID: " + aGameObjectID);
	return nullptr;
}

template<class T>
inline const bool GameObjectManager::HasComponent(const SY::UUID aGameObjectID)
{
	if (myComponentManagers.contains(&typeid(T)))
	{
		return static_cast<const ComponentManager<T>*>(myComponentManagers[&typeid(T)])->HasComponent(aGameObjectID);
	}
	return false;
}

template<class T>
inline T& GameObjectManager::GetComponent(const SY::UUID aGameObjectID)
{
	assert(myComponentManagers.contains(&typeid(T)) && "GameObjectManager::GetComponent(...) component manager doesn't exist.");
	return static_cast<ComponentManager<T>*>(myComponentManagers[&typeid(T)])->GetComponent(aGameObjectID);
}

template<class T>
void GameObjectManager::SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority)
{
	if (!myComponentManagers.contains(&typeid(T)))
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