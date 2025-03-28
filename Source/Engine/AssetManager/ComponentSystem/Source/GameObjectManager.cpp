#include "AssetManager.pch.h"

#include "../GameObjectManager.h"
#include "../GameObject.h"

#include "Editor/Editor/Core/Editor.h"
#include "Engine/AssetManager/ComponentSystem/ComponentManager.h"

GameObjectManager::~GameObjectManager() = default;

void GameObjectManager::unload()
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

	// Force add transform?
	myGameObjects.emplace(myLastID, data);
	auto object = GameObject(myLastID++, this);
	object.AddComponent<Transform>();

	return object;
}

GameObject GameObjectManager::CreateGameObject(const SY::UUID aGameObjectID)
{
	GameObjectData data;

	data.IsActive = true;
	data.Name = "GameObject(" + std::to_string(aGameObjectID) + ")";
	data.onLayer = Layer::Default;

	return CreateGameObject(aGameObjectID, data);
}

GameObject GameObjectManager::CreateGameObject(const SY::UUID aGameObjectID, const GameObjectData& data)
{
	OPTICK_EVENT();

	SY::UUID id;
	if (myGameObjects.contains(aGameObjectID))
	{
		myGameObjects.emplace(myLastID, data);
		id = myLastID;
		myLastID++;
	}
	else
	{
		myGameObjects.emplace(aGameObjectID, data);
		id = aGameObjectID;
	}

	auto object = GameObject(id, this);
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
			for (const auto& cm : myComponentManagers)
			{
				cm.second->DeleteGameObject(aGameObjectID);
			}

			myGameObjects.erase(aGameObjectID);
			EDITOR_INSTANCE.CheckSelectedForRemoved();
		}
		else
		{
			std::string err = "GameObjectManager::DeleteGameObject(): Tried to get delete a missing GameObject. ID: " +
				std::to_string(aGameObjectID);
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
	OPTICK_EVENT();
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
	OPTICK_EVENT();
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

bool GameObjectManager::GetActive(const SY::UUID aGameObjectID) const
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return false;
	}

	return myGameObjects.at(aGameObjectID).IsActive;
}

Layer GameObjectManager::GetLayer(const SY::UUID aGameObjectID) const
{
	OPTICK_EVENT();
	return myGameObjects.at(aGameObjectID).onLayer;
}

GameObject GameObjectManager::GetPlayer()
{
	return GameObject(myPlayer, this);
}

GameObject GameObjectManager::GetWorldRoot()
{
	return GameObject(myWorldRoot, this);
}

GameObject GameObjectManager::GetCamera()
{
	return GameObject(myCamera, this);
}

GameObject GameObjectManager::GetGameObject(SY::UUID anID)
{
	OPTICK_EVENT();
	if (myGameObjects.contains(anID))
	{
		return GameObject(anID, this);
	}

	// assert(false && "GameObjectManager tried to get a GameObject that doesn't exist yet! Create your GameObject first
	// "
	//                 "before trying to get it.");
	return GameObject();
}

bool GameObjectManager::HasGameObject(SY::UUID anID) const
{
	if (myGameObjects.contains(anID) && anID.IsValid())
	{
		return true;
	}
	return false;
}

bool GameObjectManager::HasGameObject(const GameObject& anID) const
{
	return HasGameObject(anID.myID);
}

void GameObjectManager::OnColliderEnter(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
	OPTICK_EVENT();
	for (const auto& cm : myComponentManagers | std::views::values)
	{
		cm->OnColliderEnter(aFirstID, aTargetID);
	}
}

void GameObjectManager::OnColliderExit(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
	OPTICK_EVENT();
	for (const auto& cm : myComponentManagers | std::views::values)
	{
		cm->OnColliderExit(aFirstID, aTargetID);
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
}

void GameObjectManager::SetLayer(const SY::UUID aGameObjectID, const Layer aLayer)
{
	myGameObjects.at(aGameObjectID).onLayer = aLayer;
}

Component* GameObjectManager::AddBaseComponent(const SY::UUID aGameObjectID, const Component* aComponent)
{
	OPTICK_EVENT();
	if (!myComponentManagers.contains(aComponent->GetTypeInfo().Name()))
	{
		throw std::exception("You reached a dead end of my mind because i have no clue how i should do this");
	}

	return myComponentManagers[aComponent->GetTypeInfo().Name()]->AddComponent(aGameObjectID, aComponent);
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
	assert(HasComponent<Camera>(myLastID - 1) == true &&
		"You really should add a camera to staticly saved scene object camera..");
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
}

void GameObjectManager::SortUpdateOrder()
{
	OPTICK_EVENT();
	myUpdateOrder.clear();
	for (auto& cm : myComponentManagers)
	{
		myUpdateOrder.push_back(cm);
	}
	std::ranges::sort(myUpdateOrder, [](const auto& aFirst, const auto& aSecond)
	{
		return static_cast<int>(aFirst.second->GetUpdatePriority()) <
			static_cast<int>(aSecond.second->GetUpdatePriority());
	});
}

void GameObjectManager::DeleteObjects()
{
	OPTICK_EVENT();
	for (size_t i = 0; i < myObjectsToDelete.size(); i++)
	{
		if (myGameObjects.contains(myObjectsToDelete[i]))
		{
			for (const auto& cm : myComponentManagers)
			{
				cm.second->DeleteGameObject(myObjectsToDelete[i]);
			}
			myGameObjects.erase(myObjectsToDelete[i]);
			EDITOR_INSTANCE.CheckSelectedForRemoved();
		}
		else
		{
			std::string err = "GameObjectManager::DeleteGameObject(): Tried to get delete a missing GameObject. ID: " +
				std::to_string(myObjectsToDelete[i]);
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
	for (const auto& cm : myComponentManagers)
	{
		cm.second->OnSiblingChanged(anID, SourceClass);
	}
}

void GameObjectManager::Merge(const GameObjectManager& other)
{
	OPTICK_EVENT();

	unsigned maxId = myLastID;
	for (const auto& [key, value] : other.myGameObjects)
	{
		myGameObjects.emplace(myLastID + key, value);
		maxId = std::max(maxId, myLastID + key);
	}
	for (const auto& [key, value] : other.myComponentManagers)
	{
		if (myComponentManagers.contains(key))
		{
			myComponentManagers.at(key)->Merge(value.get(), myLastID);
		}
		else
		{
			myComponentManagers[key] = value->Clone();
			myComponentManagers[key]->RebaseSelf(SY::UUID(myLastID), this);
			SortUpdateOrder();
		}
	}
	myLastID = maxId;
	myWorldRoot ? myWorldRoot : other.myWorldRoot;
	myPlayer ? myPlayer : other.myPlayer;
	myCamera ? myCamera : other.myCamera;
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
		LOGGER.Warn(std::format("Name is set to be longer then the max limit of 128, name is reduced to {}",
		                        stringCopy)); // Why do i need to cstr this??
		myGameObjects.at(aGameObjectID).Name = stringCopy;
		return;
	}
	myGameObjects.at(aGameObjectID).Name = name;
}

GameObjectManager::GameObjectData GameObjectManager::GetData(const SY::UUID aGameObjectID)
{
	OPTICK_EVENT();
	if (!myGameObjects.contains(aGameObjectID))
	{
		return {};
	}

	return myGameObjects.at(aGameObjectID);
}
