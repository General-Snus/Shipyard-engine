#pragma once
#ifndef GameObjectManagerDef
#define GameObjectManagerDef
#include "ComponentManager.h"
#include "External/Optick/include/optick.h"
#include "UUID.h"
#include <Engine/AssetManager/Enums.h>
#include <memory>
#include <string>
#include <unordered_map>

class Component;
class Scene;
class GameObject;

class GameObjectManager
{
	friend class Scene;

  public:
	struct GameObjectData
	{
		bool IsActive = true;
		bool IsVisibleInHierarcy = true;
		Layer onLayer = Layer::Default;
		std::string Name;
	};
	friend class GameObject;
	friend class GameobjectAdded;
	friend class GameobjectDeleted;
	GameObjectManager(Scene &ref) : m_OwnerScene(ref) {};
	~GameObjectManager();
	void unload();
	GameObject CreateGameObject();
	GameObject CreateGameObject(const SY::UUID aGameObjectID);

	void DeleteGameObject(const SY::UUID aGameObjectID, bool force = false);
	void DeleteGameObject(const GameObject aGameObject, bool force = false);

	template <class T> void RemoveComponent(const SY::UUID aGameObjectID);

	template <class T> T &AddComponent(const SY::UUID aGameObjectID);

	template <class T> T &AddComponent(const SY::UUID aGameObjectID, const T &aComponent);
	Component *AddBaseComponent(const SY::UUID aGameObjectID, const Component *aComponent);

	template <class T, typename... Args> T &AddComponent(const SY::UUID aGameObjectID, Args... someParameters);

	template <class T> std::vector<T> &GetAllComponents(); 
	template <class T> bool GetReadOnly(std::vector<T> & componentVector) const; 

	template <class T> T *TryGetComponent(const SY::UUID aGameObjectID);

	template <class T> const bool HasComponent(const SY::UUID aGameObjectID);

	template <class T> T &GetComponent(const SY::UUID aGameObjectID);

	std::vector<Component *> GetAllAttachedComponents(SY::UUID aGameObjectID);
	std::vector<Component *> CopyAllAttachedComponents(SY::UUID aGameObjectID);

	bool GetActive(const SY::UUID aGameObjectID) const;
	Layer GetLayer(const SY::UUID aGameObjectID) const;

	GameObject GetWorldRoot();
	GameObject GetPlayer();
	GameObject GetCamera();
	GameObject GetGameObject(SY::UUID anID);
	bool HasGameObject(SY::UUID anID) const;
	bool HasGameObject(const GameObject &anID) const;

	void OnColliderEnter(const SY::UUID aFirstID, const SY::UUID aTargetID);
	void OnColliderExit(const SY::UUID aFirstID, const SY::UUID aTargetID);
	void SetActive(const SY::UUID aGameObjectID, const bool aState);
	void SetLayer(const SY::UUID aGameObjectID, const Layer alayer);

	void SetLastGOAsPlayer();
	void SetLastGOAsWorld();
	void SetLastGOAsCamera();

	// hack used to add and remove same id in same frame
	void CustomOrderUpdate();
	const std::unordered_map<SY::UUID, GameObjectData> &GetAllGameObjects();

	template <class T> void SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority);
	void Update();

	// Dev function to hide object within a scene from the editor
	void SetIsVisibleInHierarchy(const SY::UUID aGameObjectID, bool setValue);

	// Please dont call this for an other object than your own
	void OnSiblingChanged(SY::UUID anID, const std::type_info *SourceClass = nullptr);

  private:
	void Merge(const GameObjectManager &other);
	std::string GetName(const SY::UUID aGameObjectID);
	void SetName(const std::string &name, const SY::UUID aGameObjectID);

	GameObjectData GetData(const SY::UUID aGameObjectID);
	GameObject CreateGameObject(const SY::UUID aGameObjectID, const GameObjectData &data);

	template <class T> void AddManager();
	void SortUpdateOrder();
	void DeleteObjects();
	void AddObjects();

	Scene &m_OwnerScene;

	std::unordered_map<std::string, std::shared_ptr<ComponentManagerBase>> myComponentManagers = {};
	std::unordered_map<SY::UUID, GameObjectData> myGameObjects = {};
	std::vector<std::pair<std::string, std::shared_ptr<ComponentManagerBase>>> myUpdateOrder = {};

	std::vector<SY::UUID> myObjectsToDelete = {};
	std::vector<SY::UUID> myObjectsToAdd = {};
	unsigned int myLastID = 1;
	SY::UUID myWorldRoot{};
	SY::UUID myPlayer{};
	SY::UUID myCamera{};

  private:
#define ReflectedComponentName(T) refl::reflect<T>().name.str()
};

template <class T> void GameObjectManager::RemoveComponent(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (myComponentManagers.contains(ReflectedComponentName(T)))
	{
		std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
			->DeleteGameObject(aGameObjectID);
	}
}

template <class T> T &GameObjectManager::AddComponent(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T))) // TODO !FUCK THE T TYPE IS
																  // DIFFERENT FOR DIFFERENT
																  // APPLICATIONS, USE REFLECTION
																  // DATA INSTEAD YOU FOOOL
	{
		AddManager<T>();
	}
	return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
		->AddComponent(aGameObjectID);
}

template <class T> T &GameObjectManager::AddComponent(const SY::UUID aGameObjectID, const T &aComponent)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T)))
	{
		AddManager<T>();
	}
	return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
		->AddComponent(aGameObjectID, aComponent);
}

template <class T, typename... Args>
T &GameObjectManager::AddComponent(const SY::UUID aGameObjectID, Args... someParameters)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T)))
	{
		AddManager<T>();
	}
	return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
		->AddComponent(aGameObjectID, someParameters...);
}

template <class T> std::vector<T> &GameObjectManager::GetAllComponents()
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T)))
	{
		AddManager<T>();
	}
	return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
		->GetAllComponents();
}

template <class T> inline bool GameObjectManager::GetReadOnly(std::vector<T> &componentVector) const
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T)))
	{
		return false;
	}
	std::shared_ptr<const ComponentManager<T>> cmpManager =
		std::static_pointer_cast<const ComponentManager<T>>(myComponentManagers.at(ReflectedComponentName(T)));
	
	componentVector = cmpManager->GetAllComponents();

	return true;
}
 

template <class T> T *GameObjectManager::TryGetComponent(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (myComponentManagers.contains(ReflectedComponentName(T)))
	{
		return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
			->TryGetComponent(aGameObjectID);
	}
	return nullptr;
}

template <class T> inline const bool GameObjectManager::HasComponent(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (myComponentManagers.contains(ReflectedComponentName(T)))
	{
		return std::static_pointer_cast<const ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
			->HasComponent(aGameObjectID);
	}
	return false;
}

template <class T> inline T &GameObjectManager::GetComponent(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	assert(myComponentManagers.contains(ReflectedComponentName(T)) &&
		   "GameObjectManager::GetComponent(...) component manager doesn't exist.");
	return std::static_pointer_cast<ComponentManager<T>>(myComponentManagers[ReflectedComponentName(T)])
		->GetComponent(aGameObjectID);
}

template <class T> void GameObjectManager::SetUpdatePriority(const ComponentManagerBase::UpdatePriority aPriority)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(ReflectedComponentName(T)))
	{
		myComponentManagers[ReflectedComponentName(T)] = std::make_shared<ComponentManager<T>>(this);
	}
	myComponentManagers[ReflectedComponentName(T)]->SetUpdatePriority(aPriority);
	SortUpdateOrder();
}

template <class T> void GameObjectManager::AddManager()
{
	OPTICK_EVENT();
	auto var = ReflectedComponentName(T);
	myComponentManagers[var] = std::make_shared<ComponentManager<T>>(this);
	SortUpdateOrder();
}
#endif
