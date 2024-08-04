#include "Engine/AssetManager/AssetManager.pch.h"

#include "../GameObject.h"
#include "../GameObjectManager.h"  
#include "Engine/AssetManager/ComponentSystem/ComponentManager.h"

GameObjectManager::~GameObjectManager()
{
	for (auto& [key, cm] : myComponentManagers)
	{
		cm->Destroy();
	}
}

GameObject GameObjectManager::CreateGameObject()
{
	OPTICK_EVENT();
	GameObjectData data;

	data.IsActive = true;
	data.Name = "GameObject(" + std::to_string(myLastID) + ")";
	data.onLayer = Layer::Default;

	//Force add transform?
	myGameObjects.emplace(myLastID, data);
	auto object = GameObject(myLastID++, this);
	object.AddComponent<Transform>();

	return object;
}

GameObject GameObjectManager::CreateGameObject(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	GameObjectData data;

	data.IsActive = true;
	data.Name = "GameObject(" + std::to_string(myLastID) + ")";
	data.onLayer = Layer::Default;

	if (myGameObjects.contains(aGameObjectID))
		//Force add transform?
		myGameObjects.emplace(myLastID, data);
	auto object = GameObject(myLastID++, this);
	object.AddComponent<Transform>();

	return object;
}

void GameObjectManager::DeleteGameObject(const SY::UUID aGameObjectID, bool force)
{
	OPTICK_EVENT();
	if (force)
	{
		if (myGameObjects.contains(aGameObjectID))
		{
			for (auto& cm : myComponentManagers)
			{
				cm.second->DeleteGameObject(aGameObjectID);
			}

			myGameObjects.erase(aGameObjectID);
		}
		else
		{
			std::string err = "GameObjectManager::DeleteGameObject(): Tried to get delete a missing GameObject. ID: " + std::to_string(aGameObjectID);
			assert(false && err.c_str());
		}
		return;
	}

	myObjectsToDelete.push_back(aGameObjectID);
}

void GameObjectManager::DeleteGameObject(const GameObject aGameObject, bool force)
{
	OPTICK_EVENT();
	DeleteGameObject(aGameObject.myID, force);
}

std::vector<Component*> GameObjectManager::GetAllAttachedComponents(const SY::UUID aGameObjectID)
{
	std::vector<Component*> components;
	for (auto& [type, manager] : myComponentManagers)
	{
		auto* cmp = manager->TryGetBaseComponent(aGameObjectID);
		if (cmp)
		{
			components.emplace_back(cmp);
		}
	}
	return components;
}

std::vector<Component*> GameObjectManager::CopyAllAttachedComponents(SY::UUID aGameObjectID)
{
	std::vector<Component*> components;
	for (auto& [type, manager] : myComponentManagers)
	{
		if (const auto cmp = manager->DeepCopy(aGameObjectID))
		{
			components.emplace_back(cmp);
		}
	}
	return components;
}

bool GameObjectManager::GetActive(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return false;
	}

	return myGameObjects.at(aGameObjectID).IsActive;
}

Layer GameObjectManager::GetLayer(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	return myGameObjects.at(aGameObjectID).onLayer;
}

GameObject GameObjectManager::GetPlayer()
{
	assert(myPlayer != 0 && "GameObjectManager: Player not set yet!");
	return GameObject(myPlayer, this);
}

GameObject GameObjectManager::GetWorldRoot()
{
	assert(myWorldRoot != 0 && "GameObjectManager: WorldRoot not set yet!");
	return GameObject(myWorldRoot, this);
}

GameObject GameObjectManager::GetCamera()
{
	assert(myCamera != 0 && "GameObjectManager: Camera not set yet!");
	return GameObject(myCamera, this);
}

GameObject GameObjectManager::GetGameObject(SY::UUID anID)
{
	OPTICK_EVENT();
	if (myGameObjects.contains(anID))
	{
		return GameObject(anID, this);
	}

	assert(false && "GameObjectManager tried to get a GameObject that doesn't exist yet! Create your GameObject first before trying to get it.");
	return GameObject();
}

void GameObjectManager::CollidedWith(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
	OPTICK_EVENT();
	for (auto& cm : myComponentManagers | std::views::values)
	{
		cm->CollidedWith(aFirstID, aTargetID);
	}
}

void GameObjectManager::SetActive(const SY::UUID aGameObjectID, const bool aState)
{
	OPTICK_EVENT();
	if (myGameObjects.contains(aGameObjectID))
	{
		myGameObjects[aGameObjectID].IsActive = aState;
		return;
	}
	assert(false && "GameObjectManager: Tried to set active on a missing GameObject. ID: " + aGameObjectID);
	//ERROR_PRINT("GameObjectManager: Tried to set active on a missing GameObject. ID: " + aGameObjectID);
}

void GameObjectManager::SetLayer(const SY::UUID aGameObjectID, const Layer aLayer)
{
	myGameObjects.at(aGameObjectID).onLayer = aLayer;
}
 
  Component* GameObjectManager::AddBaseComponent(const SY::UUID aGameObjectID, const Component* aComponent)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(aComponent->GetTypeInfo().TypeID()))
	{
		throw std::exception("You reached a dead end of my mind because i have no clue how i should do this");
	}

	return myComponentManagers[aComponent->GetTypeInfo().TypeID()]->AddComponent(aGameObjectID, aComponent);
}

void GameObjectManager::SetLastGOAsPlayer()
{
	myPlayer = myLastID - 1;
}
void GameObjectManager::SetLastGOAsWorld()
{
	myWorldRoot = myLastID - 1;
}
void GameObjectManager::SetLastGOAsCamera()
{
	myCamera = myLastID - 1;
}

void GameObjectManager::CustomOrderUpdate()
{
	OPTICK_EVENT();
	DeleteObjects();
}

const std::unordered_map<SY::UUID, GameObjectManager::GameObjectData>& GameObjectManager::GetAllGameObjects()
{
	return myGameObjects;
}

void GameObjectManager::Update()
{
	OPTICK_EVENT();
	DeleteObjects();
	AddObjects();
	for (size_t i = 0; i < myUpdateOrder.size(); i++)
	{
		myUpdateOrder[i].second->Update();
	}
	for (size_t i = 0; i < myUpdateOrder.size(); i++)
	{
		myUpdateOrder[i].second->Render();
	}
}

void GameObjectManager::SortUpdateOrder()
{
	OPTICK_EVENT();
	myUpdateOrder.clear();
	for (auto& cm : myComponentManagers)
	{
		myUpdateOrder.push_back(cm);
	}
	std::sort(myUpdateOrder.begin(), myUpdateOrder.end(),
		[](auto& aFirst, auto& aSecond)
		{
			return static_cast<int>(aFirst.second->GetUpdatePriority()) < static_cast<int>(aSecond.second->GetUpdatePriority());
		});
}



void GameObjectManager::DeleteObjects()
{
	OPTICK_EVENT();
	for (size_t i = 0; i < myObjectsToDelete.size(); i++)
	{
		if (myGameObjects.contains(myObjectsToDelete[i]))
		{
			for (auto& cm : myComponentManagers)
			{
				cm.second->DeleteGameObject(myObjectsToDelete[i]);
			}
			myGameObjects.erase(myObjectsToDelete[i]);
		}
		else
		{
			std::string err = "GameObjectManager::DeleteGameObject(): Tried to get delete a missing GameObject. ID: " + std::to_string(myObjectsToDelete[i]);
			assert(false && err.c_str());
		}
	}

	myObjectsToDelete.clear();
}

void GameObjectManager::AddObjects()
{
	OPTICK_EVENT();
	for (auto& i : myObjectsToAdd)
	{
		myGameObjects.emplace(i, true);
	}
	myObjectsToAdd.clear();
}

void GameObjectManager::SetIsVisibleInHierarchy(const SY::UUID aGameObjectID, bool setValue)
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return;
	}

	myGameObjects.at(aGameObjectID).IsVisibleInHierarcy = setValue;
}

void GameObjectManager::OnSiblingChanged(SY::UUID anID, const std::type_info* SourceClass)
{
	OPTICK_EVENT();
	for (auto& cm : myComponentManagers)
	{
		cm.second->OnSiblingChanged(anID, SourceClass);
	}
}

std::string GameObjectManager::GetName(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return "";
	}

	return myGameObjects.at(aGameObjectID).Name;
}

void GameObjectManager::SetName(const std::string& name, const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return;
	}

	if (name.size() > 128)
	{
		std::string stringCopy = name;
		stringCopy.resize(128);
		Logger::Warn(std::format("Name is set to be longer then the max limit of 128, name is reduced to {}", stringCopy)); // Why do i need to cstr this??
		myGameObjects.at(aGameObjectID).Name = stringCopy;
		return;
	}
	myGameObjects.at(aGameObjectID).Name = name;
}
