#include "AssetManager.pch.h"
#include <algorithm>
#include <string>
#include "../GameObject.h"
#include "../GameObjectManager.h"

#include <Tools/Optick/src/optick.h>

GameObjectManager::~GameObjectManager()
{
	for (auto& cm : myComponentManagers)
	{
		cm.second->Destroy();
		delete cm.second;
	}
}

const GameObject GameObjectManager::CreateGameObject()
{
	myGameObjects.emplace(myLastID,true);
	return GameObject(myLastID++,this);
}

//ILLEGAL
const GameObject GameObjectManager::CreateGameObjectAt(const SY::UUID aGameObjectID)
{
	if (aGameObjectID > myLastID)
	{
		myLastID = aGameObjectID;
		myLastID++;
	}
	myGameObjects.emplace(aGameObjectID,true);
	return GameObject(aGameObjectID,this);
}

void GameObjectManager::DeleteGameObject(const SY::UUID aGameObjectID)
{
	myObjectsToDelete.push_back(aGameObjectID);
}

void GameObjectManager::DeleteGameObject(const GameObject aGameObject)
{
	myObjectsToDelete.push_back(aGameObject.myID);
}

bool GameObjectManager::GetActive(const SY::UUID aGameObjectID)
{
	return myGameObjects.at(aGameObjectID).IsActive;
}

Layer GameObjectManager::GetLayer(const SY::UUID aGameObjectID)
{
	return myGameObjects.at(aGameObjectID).onLayer;
}

GameObject GameObjectManager::GetPlayer()
{
	return GameObject(myPlayer,this);
}

GameObject GameObjectManager::GetWorldRoot()
{
	return GameObject(myWorldRoot,this);
}

GameObject GameObjectManager::GetCamera()
{
	return GameObject(myCamera,this);
}

GameObject GameObjectManager::GetGameObject(SY::UUID anID)
{
	if (myGameObjects.find(anID) != myGameObjects.end())
	{
		return GameObject(anID,this);
	}

	//assert(false && "GameObjectManager tried to get a GameObject that doesn't exist yet! Create your GameObject first before trying to get it.");
	return GameObject();
}

void GameObjectManager::CollidedWith(const SY::UUID aFirstID,const SY::UUID aTargetID)
{
	for (auto& cm : myComponentManagers)
	{
		cm.second->CollidedWith(aFirstID,aTargetID);
	}
}

void GameObjectManager::SetActive(const SY::UUID aGameObjectID,const bool aState)
{
	if (myGameObjects.find(aGameObjectID) != myGameObjects.end())
	{
		myGameObjects[aGameObjectID].IsActive = aState;
		return;
	}
	assert(false && "GameObjectManager: Tried to set active on a missing GameObject. ID: " + aGameObjectID);
	//ERROR_PRINT("GameObjectManager: Tried to set active on a missing GameObject. ID: " + aGameObjectID);
}

void GameObjectManager::SetLayer(const SY::UUID aGameObjectID,const Layer aLayer)
{
	myGameObjects.at(aGameObjectID).onLayer = aLayer;
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

void GameObjectManager::Update()
{
	OPTICK_EVENT();
	for (size_t i = 0; i < myUpdateOrder.size(); i++)
	{
		myUpdateOrder[i].second->Update();
	}
	for (size_t i = 0; i < myUpdateOrder.size(); i++)
	{
		myUpdateOrder[i].second->Render();
	}
	DeleteObjects();
}

void GameObjectManager::SortUpdateOrder()
{
	myUpdateOrder.clear();
	for (auto& cm : myComponentManagers)
	{
		myUpdateOrder.push_back(cm);
	}
	std::sort(myUpdateOrder.begin(),myUpdateOrder.end(),
		[](auto& aFirst,auto& aSecond)
		{
			return static_cast<int>(aFirst.second->GetUpdatePriority()) < static_cast<int>(aSecond.second->GetUpdatePriority());
		});
}



void GameObjectManager::DeleteObjects()
{
	OPTICK_EVENT();
	for (size_t i = 0; i < myObjectsToDelete.size(); i++)
	{
		if (myGameObjects.find(myObjectsToDelete[i]) != myGameObjects.end())
		{
			for (auto& cm : myComponentManagers)
			{
				cm.second->DeleteGameObject(myObjectsToDelete[i]);
			}
		}
		else
		{
			std::string err = "GameObjectManager::DeleteGameObject(): Tried to get delete a missing GameObject. ID: " + std::to_string(myObjectsToDelete[i]);
			assert(false && err.c_str());
		}
	}

	myObjectsToDelete.clear();
}

void GameObjectManager::OnSiblingChanged(SY::UUID anID,const std::type_info* SourceClass)
{
	for (auto& cm : myComponentManagers)
	{
		cm.second->OnSiblingChanged(anID,SourceClass);
	}
}
